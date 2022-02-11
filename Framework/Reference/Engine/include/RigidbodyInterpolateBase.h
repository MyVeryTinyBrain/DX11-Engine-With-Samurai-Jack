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

	// 보간이 비활성화상태에서 활성화상태로 전환될 때 호출합니다.
	virtual void OnEnable() = 0;

	// 물리 시뮬레이션이 끝난 후에 호출합니다.
	// 강체의 포즈를 저장합니다.
	virtual void BackupPose() = 0;

	// 물리 시뮬레이션이 시작하기 전에 호출합니다.
	// 강체의 포즈를 복원합니다.
	virtual void RollbackPose() = 0;

	// 업데이트에서 호출합니다.
	// 게임오브젝트의 포즈를 보관합니다.
	virtual void InterpolatePose() = 0;

	// 업데이트 체크에서 호출합니다.
	// 보간 중에 위치가 변경되었을 경우에 저장된 포즈를 수정합니다.
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
