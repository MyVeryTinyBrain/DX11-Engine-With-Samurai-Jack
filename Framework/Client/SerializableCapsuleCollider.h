#pragma once

#include "ISerializableCollider.h"

EDITOR_BEGIN

class SerializableCapsuleCollider : public ISerializableCollider
{
private:

	virtual void Awake() override;

private:

	virtual Collider* GetCollider() const override;
	virtual bool Raycast(ICamera* camera, V3& out_hit) const override;
	virtual string GetType() const override;
	virtual void OnImGui() override;
	virtual void OnSerialize(Json::Value& json) const override;
	virtual void OnDeserialize(const Json::Value& json) override;

private:

	CapsuleCollider* m_capsuleCollider;
};

EDITOR_END
