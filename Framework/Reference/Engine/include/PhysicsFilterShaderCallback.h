#pragma once

ENGINE_BEGIN
class PhysicsLayerManager;
class PhysicsFilterShaderCallback : public PxSimulationFilterCallback
{
public:

	PhysicsFilterShaderCallback(PhysicsLayerManager* physicsLayerManager);

	virtual ~PhysicsFilterShaderCallback();

private:

	// ���������� �浹�� �� ��翡 ���ؼ� ó���մϴ�.
	// 1. �� ��� �ڽ��� ��ġ�� ��쿡 ȣ��˴ϴ�.
	// 2. �Ӽ��� ����Ǵ� ��� ȣ��˴ϴ�.
	virtual PxFilterFlags pairFound(
		PxU32 pairID,
		PxFilterObjectAttributes attributes0, PxFilterData filterData0, const PxActor* a0, const PxShape* s0,
		PxFilterObjectAttributes attributes1, PxFilterData filterData1, const PxActor* a1, const PxShape* s1,
		PxPairFlags& pairFlags
	) override;

	// ������ �浹 ���� ��������� �˸��� �ݹ��Դϴ�.
	// �� �޼���� �浹 ���� ������ų� �ٽ� ���͸��� �� ȣ��˴ϴ�. 
	// ���� �ݹ� ������ ǥ�õ� �浹 �ֿ��� ����˴ϴ�(pairFound() �� ������ PxFilterFlag::eNOTIFY).
	virtual void pairLost(
		PxU32 pairID,
		PxFilterObjectAttributes attributes0, PxFilterData filterData0,
		PxFilterObjectAttributes attributes1, PxFilterData filterData1,
		bool objectRemoved
	) override
	{}

	// ���� ������ �����ؾ� �ϴ� ��� True�Դϴ�. �� ��� �޼��尡 �ٽ� ȣ��˴ϴ�. 
	// ���� �ùķ��̼� �ܰ迡�� �� �̻� ���� ������ �������� �ʾƾ� �ϴ� ��� False�Դϴ�. �� ��� ������ �÷��״� ���˴ϴ�.
	inline virtual bool statusChange(
		PxU32& pairID,
		PxPairFlags& pairFlags,
		PxFilterFlags& filterFlags
	) override
	{ return false; }

public:

	// �浹 ���͸��� ����� ����� ���� ���� ���̴��Դϴ�.
	static PxFilterFlags PxSimulationFilterShader(
		PxFilterObjectAttributes attributes0, PxFilterData filterData0,
		PxFilterObjectAttributes attributes1, PxFilterData filterData1,
		PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);

private:

	PhysicsLayerManager* m_physicsLayerManager = nullptr;

private:

	// �浹 �̺�Ʈ ȣ�⿡ �ʿ��� �÷��׵��Դϴ�.
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

	// �� �÷��׵��� �Ӱ谪�� �������� �ݹ��� ȣ���մϴ�.
	// eNOTIFY_THRESHOLD_FORCE_FOUND 
	// eNOTIFY_THRESHOLD_FORCE_PERSISTS 
	// eNOTIFY_THRESHOLD_FORCE_LOST 
};

ENGINE_END
