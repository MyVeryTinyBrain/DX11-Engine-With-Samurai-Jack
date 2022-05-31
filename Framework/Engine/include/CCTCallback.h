#pragma once

ENGINE_BEGIN

class CharacterController;
class PhysicsLayerManager;

class CCTCallback : public PxUserControllerHitReport, public PxControllerBehaviorCallback, public PxQueryFilterCallback, public PxControllerFilterCallback
{
public:

	void Initialize(CharacterController* characterController);
	CCTCallback() = default;
	~CCTCallback() = default;

private:

	// PxUserControllerHitReport =================================================================================

	virtual void onShapeHit(const PxControllerShapeHit& hit) override;
	virtual void onControllerHit(const PxControllersHit& hit) override;
	virtual void onObstacleHit(const PxControllerObstacleHit& hit) override;

	// PxControllerBehaviorCallback ==============================================================================

	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxShape& shape, const PxActor& actor) override;
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxController& controller) override;
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxObstacle& obstacle) override;

	// CCT vs Rigidbody ==========================================================================================

	virtual PxQueryHitType::Enum preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags) override;
	virtual PxQueryHitType::Enum postFilter(const PxFilterData& filterData, const PxQueryHit& hit) override;

	// CCT vs CCT ================================================================================================

	virtual bool filter(const PxController& a, const PxController& b) override;

	// ===========================================================================================================

	void CCTInteraction(const PxControllerShapeHit& hit);
	void AddForceAtLocalPos(PxRigidBody& body, const PxVec3& force, const PxVec3& pos, PxForceMode::Enum mode, bool wakeup = true);
	void AddForceAtPosInternal(PxRigidBody& body, const PxVec3& force, const PxVec3& pos, PxForceMode::Enum mode, bool wakeup);

	CharacterController*	m_characterController = nullptr;
};

ENGINE_END
