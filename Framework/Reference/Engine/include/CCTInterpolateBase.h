#pragma once

ENGINE_BEGIN
class CharacterController;
class Times;
class ENGINE_API CCTInterpolateBase
{
public:

	inline CCTInterpolateBase(Times* time, CharacterController* cct) : m_time(time), m_controller(cct) {}

	virtual ~CCTInterpolateBase() = default;

public:

	// ������ ��Ȱ��ȭ���¿��� Ȱ��ȭ���·� ��ȯ�� �� ȣ���մϴ�.
	virtual void OnEnable() = 0;

	// ���� �ùķ��̼��� ���� �Ŀ� ȣ���մϴ�.
	// ��ü�� ��� �����մϴ�.
	virtual void BackupPose() = 0;

	// ���� �ùķ��̼��� �����ϱ� ���� ȣ���մϴ�.
	// ��ü�� ��� �����մϴ�.
	virtual void RollbackPose() = 0;

	// ������Ʈ���� ȣ���մϴ�.
	// ���ӿ�����Ʈ�� ��� �����մϴ�.
	virtual void InterpolatePose() = 0;

	// ������Ʈ üũ���� ȣ���մϴ�.
	// ���� �߿� ��ġ�� ����Ǿ��� ��쿡 ����� ��� �����մϴ�.
	virtual void CheckPoseChange() = 0;

public:

	inline bool IsInterpolatePosition() const { return m_interpolatePosition; }

	inline void SetInterpolatePositionMode(bool value) { m_interpolatePosition = value; }

protected:

	virtual V3 CalcInterpolatePosition() const = 0;

protected:

	Times* m_time = nullptr;
	CharacterController* m_controller = nullptr;

	bool m_interpolatePosition = true;
};

ENGINE_END
