#include "EnginePCH.h"
#include "RigidbodyInterpolate.h"
#include "Rigidbody.h"
#include "Transform.h"
#include "Times.h"

void RigidbodyInterpolate::OnEnable()
{
	BackupPose();

	m_beforePosition = m_backupPosition;
	m_beforeRotation = m_backupRotation;
}

void RigidbodyInterpolate::BackupPose()
{
	m_beforePosition = m_backupPosition;
	m_beforeRotation = m_backupRotation;

	m_backupPosition = m_body->transform->position;
	m_backupRotation = m_body->transform->rotation;
}

void RigidbodyInterpolate::RollbackPose()
{
	if (m_interpolatePosition)
	{
		m_body->transform->position = m_backupPosition;
	}

	if (m_interpolateRotation)
	{
		m_body->transform->rotation = m_backupRotation;
	}
}

void RigidbodyInterpolate::InterpolatePose()
{
	if (m_interpolatePosition)
	{
		m_body->transform->position = CalcInterpolatePosition();
	}

	if (m_interpolateRotation)
	{
		m_body->transform->rotation = CalcInterpolateRotation();
	}
}

void RigidbodyInterpolate::CheckPoseChange()
{
	V3 deltaPos = m_body->transform->position - CalcInterpolatePosition();
	if (deltaPos.sqrMagnitude > Epsilon)
	{
		m_backupPosition += deltaPos;
		m_beforePosition += deltaPos;
	}

	Q deltaRotation = m_body->transform->rotation * CalcInterpolateRotation().inversed;
	float deltaAngle = Q::Angle(Q::identity(), deltaRotation);
	if (deltaAngle > Epsilon)
	{
		m_backupRotation = m_backupRotation * deltaRotation;
		m_beforeRotation = m_beforeRotation * deltaRotation;
	}
}

// accumulated: 고정 업데이트 이후 지난 시간입니다. 고정 업데이트 이후에 0으로 초기화됩니다.
// percent: 다음 고정 업데이트 이후 지난 정규화된 시간입니다.

V3 RigidbodyInterpolate::CalcInterpolatePosition() const
{
	float accumulated = m_time->GetFixedAccumulated() * m_time->GetFixedTimeScale();
	float percent = accumulated / m_time->GetFixedInterval();
	percent = Clamp01(percent);
	return V3::Lerp(m_beforePosition, m_backupPosition, percent);
}

Q RigidbodyInterpolate::CalcInterpolateRotation() const
{
	float accumulated = m_time->GetFixedAccumulated() * m_time->GetFixedTimeScale();
	float percent = accumulated / m_time->GetFixedInterval();
	percent = Clamp01(percent);
	return Q::SLerp(m_beforeRotation, m_backupRotation, percent);
}
