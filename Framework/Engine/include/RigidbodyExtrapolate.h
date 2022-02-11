#pragma once

#include "RigidbodyInterpolateBase.h"

ENGINE_BEGIN

class ENGINE_API RigidbodyExtrapolate : public RigidbodyInterpolateBase
{
public:

	inline RigidbodyExtrapolate(Times* time, Rigidbody* rigidbody) : RigidbodyInterpolateBase(time, rigidbody) {}

private:

	virtual void OnEnable() override;
	virtual void BackupPose() override;
	virtual void RollbackPose() override;
	virtual void InterpolatePose() override;
	virtual void CheckPoseChange() override;

	virtual V3 CalcInterpolatePosition() const override;
	virtual Q CalcInterpolateRotation() const override;

private:

	V3 m_backupPosition;
	Q  m_backupRotation;
};

ENGINE_END