#pragma once

#include "Component.h"
#include "ICollider.h"
#include "PhysicsSimulationCallbackType.h"

ENGINE_BEGIN
class Rigidbody;
class ENGINE_API Collider abstract : public Component, public ICollider
{
public:

	enum class CombineMode
	{
		Average = PxCombineMode::eAVERAGE,
		Min = PxCombineMode::eMIN,
		Max = PxCombineMode::eMAX,
		Multiply = PxCombineMode::eMULTIPLY,
	};

protected:

	virtual void Awake() override;
	virtual void BeforePhysicsSimulation() override;
	virtual void AfterPhysicsSimulation() override {}
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	virtual void OnDestroyed() override;

	virtual void Render() final override { if (m_debugRender)DebugRender(); }
	virtual void DebugRender() = 0;

protected:

	virtual ~Collider();

public:	

	inline bool IsTrigger() const { return m_isTrigger; }
	void SetTrigger(bool value);

	inline float GetFriction() const { return m_material->getDynamicFriction(); }
	void SetFriction(float value);

	inline float GetRestitution() const { return m_material->getRestitution(); }
	inline void SetRestitution(float value) { m_material->setRestitution(value); }

	inline Collider::CombineMode GetFrictionCombineMode() const { return (Collider::CombineMode)m_material->getFrictionCombineMode(); }
	inline void SetFrictionCombineMode(Collider::CombineMode value) { m_material->setFrictionCombineMode((PxCombineMode::Enum)value); }

	inline Collider::CombineMode GetRestituitionCombineMode() const { return (Collider::CombineMode)m_material->getRestitutionCombineMode(); }
	inline void SetRestituitionCombineMode(Collider::CombineMode value) { m_material->setRestitutionCombineMode((PxCombineMode::Enum)value); }

	_declspec(property(get = IsTrigger, put = SetTrigger)) bool isTrigger;
	_declspec(property(get = GetFriction, put = SetFriction)) float friction;
	_declspec(property(get = GetRestitution, put = SetRestitution)) float restitution;
	_declspec(property(get = GetFrictionCombineMode, put = SetFrictionCombineMode)) Collider::CombineMode frictionCombineMode;
	_declspec(property(get = GetRestituitionCombineMode, put = SetRestituitionCombineMode)) Collider::CombineMode restituionCombineMode;

public:

	inline uint32 GetIgnoreLayerBits() const { return m_ignoreLayerBits; }
	void SetIngoreLayerBits(uint32 value);

	bool IsIgnoreLayerIndex(uint8 layerIndex) const;
	void SetIgnoreLayerIndex(uint layerIndex, bool ignore);

	inline uint8 GetLayerIndex() const { return m_layerIndex; }
	void SetLayerIndex(uint8 layerIndex);

	inline bool IsValid() const { return m_valid; }

	inline bool IsDebugRenderMode() const { return m_debugRender; }
	void SetDebugRenderMode(bool value);

	_declspec(property(get = GetIgnoreLayerBits, put = SetIngoreLayerBits)) uint32 ignoreLayerBits;
	_declspec(property(get = GetLayerIndex, put = SetLayerIndex)) uint8 layerIndex;
	_declspec(property(get = IsValid)) bool isValid;
	_declspec(property(get = IsDebugRenderMode, put = SetDebugRenderMode)) bool debugRender;

public:

	Rigidbody* GetRigidbody() const;

	_declspec(property(get = GetRigidbody)) Rigidbody* rigidbody;

public:

	virtual Bounds GetBounds() const = 0;

	PxGeometryHolder GetPxGeometry() const { return m_shape->getGeometry(); }

	_declspec(property(get = GetPxGeometry)) PxGeometryHolder pxGeometry;

protected:

	virtual PxGeometryHolder CreatePxGeometry(bool& out_invalid) const = 0;

	virtual void OnDebugRenderModeChanged(bool value) {}

	virtual void ResetShape();

private:

	void FindRigidbodyAndAttach();
	void ApplyFlags();
	void ApplyPose(bool unconditionally);
	void ApplyScale(bool unconditionally);
	void ApplyLayer();
		
private:

	virtual bool IsWorking() override;

private:

	inline virtual PxShape* GetPxShape() const override { return m_shape; }

private:

	PxMaterial*		m_material = nullptr;

	PxShape*		m_shape = nullptr;

	bool			m_isTrigger = false;

	uint32			m_ignoreLayerBits = 0;

	uint8			m_layerIndex = 0;

	V3				m_beforeLocalPosition;
	Q				m_beforeLocalRotation;
	V3				m_beforeWorldScale;

	bool			m_valid = false;

	bool			m_debugRender = false;

protected:

	Q				m_defaultRotation = Q::identity();

public:

	Delegate<void(const Collision&)> OnCollisionEnter;
	Delegate<void(const Collision&)> OnCollisionStay;
	Delegate<void(const Collision&)> OnCollisionExit;
};

ENGINE_END
