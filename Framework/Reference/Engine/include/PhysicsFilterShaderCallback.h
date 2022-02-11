#pragma once

ENGINE_BEGIN
class PhysicsLayerManager;
class PhysicsFilterShaderCallback : public PxSimulationFilterCallback
{
public:

	PhysicsFilterShaderCallback(PhysicsLayerManager* physicsLayerManager);

	virtual ~PhysicsFilterShaderCallback();

private:

	// 잠재적으로 충돌할 두 모양에 대해서 처리합니다.
	// 1. 두 경계 박스가 겹치는 경우에 호출됩니다.
	// 2. 속성이 변경되는 경우 호출됩니다.
	virtual PxFilterFlags pairFound(
		PxU32 pairID,
		PxFilterObjectAttributes attributes0, PxFilterData filterData0, const PxActor* a0, const PxShape* s0,
		PxFilterObjectAttributes attributes1, PxFilterData filterData1, const PxActor* a1, const PxShape* s1,
		PxPairFlags& pairFlags
	) override;

	// 추적된 충돌 쌍이 사라졌음을 알리는 콜백입니다.
	// 이 메서드는 충돌 쌍이 사라지거나 다시 필터링될 때 호출됩니다. 
	// 필터 콜백 쌍으로 표시된 충돌 쌍에만 적용됩니다(pairFound() 에 설정된 PxFilterFlag::eNOTIFY).
	virtual void pairLost(
		PxU32 pairID,
		PxFilterObjectAttributes attributes0, PxFilterData filterData0,
		PxFilterObjectAttributes attributes1, PxFilterData filterData1,
		bool objectRemoved
	) override
	{}

	// 변경 사항을 적용해야 하는 경우 True입니다. 이 경우 메서드가 다시 호출됩니다. 
	// 현재 시뮬레이션 단계에서 더 이상 상태 변경을 수행하지 않아야 하는 경우 False입니다. 이 경우 제공된 플래그는 폐기됩니다.
	inline virtual bool statusChange(
		PxU32& pairID,
		PxPairFlags& pairFlags,
		PxFilterFlags& filterFlags
	) override
	{ return false; }

public:

	// 충돌 필터링에 사용할 사용자 정의 필터 셰이더입니다.
	static PxFilterFlags PxSimulationFilterShader(
		PxFilterObjectAttributes attributes0, PxFilterData filterData0,
		PxFilterObjectAttributes attributes1, PxFilterData filterData1,
		PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);

private:

	PhysicsLayerManager* m_physicsLayerManager = nullptr;

private:

	// 충돌 이벤트 호출에 필요한 플래그들입니다.
	const PxU32 eNotifyFlags =
		PxPairFlag::eDETECT_DISCRETE_CONTACT |
		PxPairFlag::eNOTIFY_TOUCH_FOUND |
		PxPairFlag::eNOTIFY_TOUCH_LOST |
		PxPairFlag::eNOTIFY_CONTACT_POINTS;

	const PxPairFlags eTriggerFlags =
		PxPairFlag::eTRIGGER_DEFAULT |
		(PxPairFlag::Enum)eNotifyFlags;

	const PxPairFlags eContactFlags =
		PxPairFlag::eCONTACT_DEFAULT |
		(PxPairFlag::Enum)eNotifyFlags |
		PxPairFlag::eDETECT_CCD_CONTACT |
		PxPairFlag::eNOTIFY_TOUCH_CCD |
		PxPairFlag::eNOTIFY_TOUCH_PERSISTS;

	// 이 플래그들은 임계값을 기준으로 콜백을 호출합니다.
	// eNOTIFY_THRESHOLD_FORCE_FOUND 
	// eNOTIFY_THRESHOLD_FORCE_PERSISTS 
	// eNOTIFY_THRESHOLD_FORCE_LOST 
};

ENGINE_END
