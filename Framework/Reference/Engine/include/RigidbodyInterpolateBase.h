#pragma once

ENGINE_BEGIN
class Rigidbody;
class Times;
class ENGINE_API RigidbodyInterpolateBase
{
public:

	inline RigidbodyInterpolateBase(Times* time, Rigidbody* rigidbody) : m_time(time), m_body(rigidbody) {}

	virtual ~RigidbodyInterpolateBase() = default;

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

	inline bool IsInterpolateRotation() const { return m_interpolateRotation; }

	inline void SetInterpolatePositionMode(bool value) { m_interpolatePosition = value; }

	inline void SetInterpolateRotationMode(bool value) { m_interpolateRotation = value; }

protected:

	virtual V3 CalcInterpolatePosition() const = 0;

	virtual Q  CalcInterpolateRotation() const = 0;

protected:

	Times* m_time = nullptr;
	Rigidbody* m_body = nullptr;

	bool m_interpolatePosition = true;
	bool m_interpolateRotation = false;
};

ENGINE_END
