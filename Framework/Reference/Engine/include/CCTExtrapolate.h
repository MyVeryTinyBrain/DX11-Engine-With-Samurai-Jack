#pragma once

#include "CCTInterpolateBase.h"

ENGINE_BEGIN

class ENGINE_API CCTExtrapolate : public CCTInterpolateBase
{
public:

	inline CCTExtrapolate(Times* time, CharacterController* cct) : CCTInterpolateBase(time, cct) {}

private:

	virtual void OnEnable() override;
	virtual void BackupPose() override;
	virtual void RollbackPose() override;
	virtual void InterpolatePose() override;
	virtual void CheckPoseChange() override;

	virtual V3 CalcInterpolatePosition() const override;

private:

	V3 m_backupPosition;
	Q  m_backupRotation;
};

ENGINE_END