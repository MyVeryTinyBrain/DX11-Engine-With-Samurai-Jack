#pragma once

#include "IPhysicsObject.h"
#include "Component.h"
#include "Ref.h"

ENGINE_BEGIN
class Collider;
class Rigidbody;
class CapsuleCollider;
class CCTInterpolateBase;

struct CCTCollision
{
	V3			point;
	V3			normal;
	Collider*	HitCollider;
};

class ICharacterController abstract
{
public:

	virtual void OnHitShape(const PxControllerShapeHit& hit) = 0;
	virtual void OnGroundHit(const V3& normal) = 0;
	virtual PxCapsuleController* GetController() const = 0;
};

class ENGINE_API CharacterController : public ICharacterController, public Component, public IPhysicsObject
{
public:

	enum class Interpolate { None, Interpolate, Extrapolate };

protected:

	virtual void Awake() override;
	virtual void BeforePhysicsSimulation() override;
	virtual void AfterPhysicsSimulation() override;
	virtual void BeforePhysicsSimulationOnce() override;
	virtual void AfterPhysicsSimulationOnce() override;
	virtual void Update() override;
	virtual void FixedUpdate() override;
	virtual void PreUpdate() override;
	virtual void PostUpdate() override;
	virtual void OnDestroyed() override;

	virtual bool IsWorking() const override;

public:

	Ref<Rigidbody> GetRigidbody() const;
	Ref<CapsuleCollider> GetCapsuleCollider() const;

	_declspec(property(get = GetRigidbody)) Ref<Rigidbody> rigidbody;
	_declspec(property(get = GetCapsuleCollider)) Ref<CapsuleCollider> capsuleCollider;

public:

	float GetContactOffset() const;
	void SetContactOffset(float value);

	float GetRadius() const;
	void SetRadius(float value);

	float GetHeight() const;
	void SetHeight(float value);

	bool IsAutoClimbMode() const;
	void SetAutoClimbMode(bool value);

	float GetStepOffset() const;
	void SetStepOffset(float value);

	float GetSlopeLimit() const;
	void SetSlopeLimit(float cosine);

	float GetSlopeLimitAsAngle() const;
	void SetSlopeLimitAsAngle(float angle);

	bool IsSlideMode() const;
	void SetSlideMode(bool value);

	V3 GetFootPosition() const;
	void SetFootPosition(const V3& value);

	V3 GetUpDirection() const;

	_declspec(property(get = GetContactOffset, put = SetContactOffset)) float contactOffset;
	_declspec(property(get = GetRadius, put = SetRadius)) float radius;
	_declspec(property(get = GetHeight, put = SetHeight)) float height;
	_declspec(property(get = IsAutoClimbMode, put = SetAutoClimbMode)) bool autoClimbMode;
	_declspec(property(get = GetStepOffset, put = SetStepOffset)) float stepOffset;
	_declspec(property(get = GetSlopeLimit, put = SetSlopeLimit)) float slopeLimit;
	_declspec(property(get = GetSlopeLimitAsAngle, put = SetSlopeLimitAsAngle)) float slopeLimitAsAngle;
	_declspec(property(get = IsSlideMode, put = SetSlideMode)) bool slideMode;
	_declspec(property(get = GetFootPosition, put = SetFootPosition)) V3 footPosition;
	_declspec(property(get = GetUpDirection)) V3 upDirection;

public:

	// delta만큼 이동합니다.
	void Move(const V3& delta, float deltaTime);

	// delta만큼 표면에 따라 이동합니다.
	// 평지가 아닌 경사진 곳에서 일정하게 이동하도록 도와줍니다.
	// 내리막을 내려갈때 뜨는 현상을 억제할수 있습니다.
	void MoveOnGround(const V3& delta, float deltaTime);

	// 위치를 즉시 이동시킵니다.
	// 위치 이동은 이 함수를 이용하는 것이 권장됩니다.
	void Teleport(const V3& position);

	// velocity만큼 점프합니다.
	// velocity를 이용한 점프보다 안정적입니다.
	void Jump(const V3& velocity);

	// 속도를 적용합니다.
	// 속도에 의한 이동은 물리 고정 업데이트에서 이루어집니다.

	inline const V3& GetVelocity() const { return m_velocity; }
	inline void SetVelocity(const V3& value) { m_velocity = value; }

	// 속도 사용시 속도의 감쇠값입니다.

	inline float GetDamping() const { return m_damping; }
	inline void SetDamping(float value) { m_damping = value; }

	// 중력 누적량의 스케일입니다.
	// 0으로 설정하면 중력을 사용하지 않습니다.
	// 큰 값을 설정하면 중력이 더 강하게 작용합니다.

	inline float GetGravityScale() const { return m_gravityScale; }
	inline void SetGravityScale(float value) { m_gravityScale = value; }

	// 속도 사용시 보간을 설정합니다.
	// 보간은 fixedUpdate 모드에서만 동작합니다.

	CharacterController::Interpolate GetInterpolateMode() const;
	void SetInterpolateMode(CharacterController::Interpolate mode);

	inline bool IsFixedUpdate() const { return m_fixedUpdate; }
	inline void SetFixedUpdateMode(bool value) { m_fixedUpdate = value; }

	_declspec(property(get = GetVelocity, put = SetVelocity)) const V3& velocity;
	_declspec(property(get = GetDamping, put = SetDamping)) float damping;
	_declspec(property(get = GetGravityScale, put = SetGravityScale)) float gravityScale;
	_declspec(property(get = GetInterpolateMode, put = SetInterpolateMode)) CharacterController::Interpolate interpolate;
	_declspec(property(get = IsFixedUpdate, put = SetFixedUpdateMode)) bool fixedUpdate;

public:

	bool IsGrounded() const { return m_groundHit; }

	_declspec(property(get = IsGrounded)) bool isGrounded;

private:

	// Simulation마다 호출됩니다.

	virtual void OnHitShape(const PxControllerShapeHit& hit) override;	
	virtual void OnGroundHit(const V3& normal) override;
	inline virtual PxCapsuleController* GetController() const override { return m_controller; }

	void NotifyCollisions();
	void ClearCollisions();

private:

	inline virtual bool IsCCTComponent() const override { return true; }

private:

	static PxCapsuleControllerDesc CreateInitDesc(PxPhysics* physics);
	void UpdateController(float deltaTime);
	void ApplyPositionFromCCT();
	void CCTMove(const V3& delta, float deltaTime);
	bool IsValid() const;

private:

	PxControllerFilters		m_filters;

	PxRigidDynamic*			m_actor = nullptr;
	PxShape*				m_shape = nullptr;

	PxCapsuleController*	m_controller = nullptr;
	Ref<Rigidbody>			m_rigidbody;
	Ref<CapsuleCollider>	m_collider;
	class CCTCallback*		m_callback = nullptr;

	bool					m_fixedUpdate = false;
	V3						m_velocity;
	float					m_damping = 1.0f;
	float					m_gravityScale = 1.0f;
	CCTInterpolateBase*		m_currentInterpolate = nullptr;
	CCTInterpolateBase*		m_interpolater = nullptr;
	CCTInterpolateBase*		m_extrapolater = nullptr;

	bool					m_groundHit = true;
	bool					m_prevGroundHit = true;
	bool					m_isJump = false;
	bool					m_isJumpFrame = false;

	vector<CCTCollision>	m_collisions;

public:

	// 캐릭터 컨트롤러가 다른 객체에 닿아있는 동안 호출됩니다.
	// 물리 업데이트마다 축척됩니다.
	delegate<void(const CCTCollision&)> OnCollision;

	// 캐릭터 컨트롤러가 지면에 착지한 경우 호출됩니다.
	delegate<void()> OnLanded;
};

ENGINE_END
