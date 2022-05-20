#pragma once

#include "Component.h"
#include "IRigidbody.h"

ENGINE_BEGIN
class RigidbodyInterpolateBase;
class ENGINE_API Rigidbody : public Component, public IRigidbody
{
public:

	enum class Interpolate { None, Interpolate, Extrapolate };
	enum class Axis
	{
		None = 0,
		X = 1 << 0,
		Y = 1 << 1,
		Z = 1 << 2,
		All = X | Y | Z,
	};
	enum class ForceMode
	{
		Impulse = PxForceMode::eIMPULSE,
		Force = PxForceMode::eFORCE,
		Acceleration = PxForceMode::eACCELERATION,
		Velocity = PxForceMode::eVELOCITY_CHANGE,
	};

private:

	virtual void Awake() override;
	virtual void Awake(void* arg/* PxRigidDynamic from CCT */) override;
	virtual void Start() override;
	virtual void BeforePhysicsSimulation() override;
	virtual void AfterPhysicsSimulation() override;
	virtual void PostFixedUpdate() override;
	virtual void PreUpdate() override;
	virtual void PostUpdate() override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	virtual void OnDestroyed() override;

protected:

	virtual ~Rigidbody();

public:

	inline bool IsUseGravity() const { return !m_body->getActorFlags().isSet(PxActorFlag::eDISABLE_GRAVITY); }
	inline void SetUseGravity(bool value) { m_body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !value); }

	inline bool IsKinematic() const { return m_body->getRigidBodyFlags().isSet(PxRigidBodyFlag::eKINEMATIC); }
	inline void SetKienmatic(bool value) { m_body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, value); }

	inline bool IsContinousDetectionMode() const { return m_continous; }
	void SetContinousDetctionMode(bool value);

	inline void Sleep() { if (!IsKinematic()) { m_body->putToSleep(); } }
	inline void WakeUp() { if (!IsKinematic()) { m_body->wakeUp(); } }
	inline bool IsSleep() const { return m_body->isSleeping(); }

	bool IsLockedRotate(Rigidbody::Axis axes);
	void SetRotationLock(Rigidbody::Axis axes, bool value);
	bool IsLockedTranslate(Rigidbody::Axis axes);
	void SetTranslationLock(Rigidbody::Axis axes, bool value);

	Rigidbody::Interpolate GetInterpolateMode() const;
	void SetInterpolateMode(Rigidbody::Interpolate mode);

	_declspec(property(get = IsUseGravity, put = SetUseGravity)) bool useGravity;
	_declspec(property(get = IsKinematic, put = SetKienmatic)) bool kinematic;
	_declspec(property(get = IsContinousDetectionMode, put = SetContinousDetctionMode)) bool continousDection;
	_declspec(property(get = IsSleep)) bool isSleep;
	_declspec(property(get = GetInterpolateMode, put = SetInterpolateMode)) Rigidbody::Interpolate interpolate;

public:

	inline float GetSleepThresholder() const { return m_body->getSleepThreshold(); }
	inline void SetSleepThresholder(float value) { m_body->setSleepThreshold(Max(0.0f, value)); }

	inline float GetMass() const { return m_body->getMass(); }
	inline void SetMass(float value) { m_body->setMass(Max(0.0f, value)); }

	inline float GetLinearDamping() const { return m_body->getLinearDamping(); }
	inline void SetLinearDamping(float value) { m_body->setLinearDamping(Max(0.0f, value)); }

	inline float GetAngularDamping() const { return m_body->getAngularDamping(); }
	inline void SetAngularDamping(float value) { m_body->setAngularDamping(value); }

	inline float GetMaxLinearVelocity() const { return m_body->getMaxLinearVelocity(); }
	inline void SetMaxLinearVelocity(float value) { m_body->setMaxLinearVelocity(Max(0.0f, value)); }

	inline float GetMaxAngularVelocity() const { return m_body->getMaxAngularVelocity(); }
	inline void SetMaxAngularVelocity(float value) { m_body->setMaxAngularVelocity(Max(0.0f, value)); }

	_declspec(property(get = GetSleepThresholder, put = SetSleepThresholder)) float sleepThresholder;
	_declspec(property(get = GetMass, put = SetMass)) float mass;
	_declspec(property(get = GetLinearDamping, put = SetLinearDamping)) float linearDamping;
	_declspec(property(get = GetAngularDamping, put = SetAngularDamping)) float angularDamping;
	_declspec(property(get = GetMaxLinearVelocity, put = SetMaxLinearVelocity)) float maxLinearVelocity;
	_declspec(property(get = GetMaxAngularVelocity, put = SetMaxAngularVelocity)) float maxAngularVelocity;

public:

	V3 GetVelocity() const;
	void SetVelocity(const V3& velocity);

	V3 GetAngularVelocity() const;
	void SetAngularVelocity(const V3& angularVelocity);

	void AddForce(const V3& force, Rigidbody::ForceMode forceMode);
	void ClearForce(Rigidbody::ForceMode forceMode);

	_declspec(property(get = GetVelocity, put = SetVelocity)) V3 velocity;
	_declspec(property(get = GetAngularVelocity, put = SetAngularVelocity)) V3 angularVelocity;

public:

	inline void ReAttachColliders() { AttachAll(); }
	inline void UpdateTransformToRigidbody() { ApplyBodyTransformFromGameObject(); }
	inline void UpdateTransformToGameObject() { ApplyGameObjectTransfromFromBody(); }

public:

	inline virtual bool IsCCTComponent() const override { return m_isCCTComponent; }

private:

	void AttachAll();
	void DetachAll();
	void ApplyFlags();
	void ApplyBodyTransformFromGameObject();
	void ApplyGameObjectTransfromFromBody();

private:

	inline virtual PxRigidDynamic* GetPxRigidDynamic() const override { return m_body; }

private:

	virtual bool IsWorking() const override;

private:

	PxRigidDynamic* m_body = nullptr;

	bool m_continous = false;

	RigidbodyInterpolateBase* m_currentInterpolate = nullptr;
	RigidbodyInterpolateBase* m_interpolater = nullptr;
	RigidbodyInterpolateBase* m_extrapolater = nullptr;

	bool m_isCCTComponent = false;
};

ENGINE_END
