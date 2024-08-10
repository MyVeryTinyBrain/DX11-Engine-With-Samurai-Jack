#include "EnginePCH.h"
#include "RigidbodyExtrapolate.h"
#include "Rigidbody.h"
#include "Transform.h"
#include "Times.h"

void RigidbodyExtrapolate::OnEnable()
{
    BackupPose();
}

void RigidbodyExtrapolate::BackupPose()
{
    m_backupPosition = m_body->transform->position;
    m_backupRotation = m_body->transform->rotation;
}

void RigidbodyExtrapolate::RollbackPose()
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

void RigidbodyExtrapolate::InterpolatePose()
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

void RigidbodyExtrapolate::CheckPoseChange()
{
	V3 deltaPos = m_body->transform->position - CalcInterpolatePosition();
	if (deltaPos.sqrMagnitude > Epsilon)
	{
		m_backupPosition += deltaPos;
	}

	Q deltaRotation = m_body->transform->rotation * CalcInterpolateRotation().inversed;
	float deltaAngle = Q::Angle(Q::identity(), deltaRotation);
	if (deltaAngle > Epsilon)
	{
		m_backupRotation = m_backupRotation * deltaRotation;
	}
}

// accumulated: 고정 업데이트 이후 지난 시간입니다. 고정 업데이트 이후에 0으로 초기화됩니다.

V3 RigidbodyExtrapolate::CalcInterpolatePosition() const
{
	float accumulated = m_time->GetFixedAccumulated() * m_time->GetFixedTimeScale();
	return m_backupPosition + m_body->velocity * accumulated;
}

Q RigidbodyExtrapolate::CalcInterpolateRotation() const
{
	float accumulated = m_time->GetFixedAccumulated() * m_time->GetFixedTimeScale();
	Q q = Q::Euler(
		m_body->angularVelocity.x * accumulated,
		m_body->angularVelocity.y * accumulated,
		m_body->angularVelocity.z * accumulated
	);
	Q res = m_backupRotation * q;
	return res;
}
