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

	inline void SetInterpolatePositionMode(bool value) { m_interpolatePosition = value; }

protected:

	virtual V3 CalcInterpolatePosition() const = 0;

protected:

	Times* m_time = nullptr;
	CharacterController* m_controller = nullptr;

	bool m_interpolatePosition = true;
};

ENGINE_END
