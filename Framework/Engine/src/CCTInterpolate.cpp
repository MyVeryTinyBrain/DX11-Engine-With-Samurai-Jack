#include "EnginePCH.h"
#include "CCTInterpolate.h"
#include "CharacterController.h"
#include "Transform.h"
#include "Times.h"

void CCTInterpolate::OnEnable()
{
	BackupPose();

	m_beforePosition = m_backupPosition;
}

void CCTInterpolate::BackupPose()
{
	m_beforePosition = m_backupPosition;

	m_backupPosition = m_controller->transform->position;
}

void CCTInterpolate::RollbackPose()
{
	if (m_interpolatePosition)
	{
		m_controller->transform->position = m_backupPosition;
	}
}

void CCTInterpolate::InterpolatePose()
{
	if (m_interpolatePosition)
	{
		m_controller->transform->position = CalcInterpolatePosition();
	}
}

void CCTInterpolate::CheckPoseChange()
{
	V3 deltaPos = m_controller->transform->position - CalcInterpolatePosition();
	if (deltaPos.sqrMagnitude > Epsilon)
	{
		m_backupPosition += deltaPos;
		m_beforePosition += deltaPos;
	}
}

V3 CCTInterpolate::CalcInterpolatePosition() const
{
	float accumulated = m_time->GetFixedAccumulated() * m_time->GetFixedTimeScale();
	float percent = accumulated / m_time->GetFixedInterval();

	percent = Clamp01(percent);

	return V3::Lerp(m_beforePosition, m_backupPosition, percent);
}
