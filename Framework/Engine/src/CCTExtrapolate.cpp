#include "EnginePCH.h"
#include "CCTExtrapolate.h"
#include "CharacterController.h"
#include "Transform.h"
#include "Times.h"

void CCTExtrapolate::OnEnable()
{
	BackupPose();
}

void CCTExtrapolate::BackupPose()
{
	m_backupPosition = m_controller->transform->position;
	m_backupRotation = m_controller->transform->rotation;
}

void CCTExtrapolate::RollbackPose()
{
	if (m_interpolatePosition)
	{
		m_controller->transform->position = m_backupPosition;
	}
}

void CCTExtrapolate::InterpolatePose()
{
	if (m_interpolatePosition)
	{
		m_controller->transform->position = CalcInterpolatePosition();
	}
}

void CCTExtrapolate::CheckPoseChange()
{
	V3 deltaPos = m_controller->transform->position - CalcInterpolatePosition();
	if (deltaPos.sqrMagnitude > Epsilon)
	{
		m_backupPosition += deltaPos;
	}
}

V3 CCTExtrapolate::CalcInterpolatePosition() const
{
	float accumulated = m_time->GetFixedAccumulated() * m_time->GetFixedTimeScale();

	return m_backupPosition + m_controller->velocity * accumulated;
}
