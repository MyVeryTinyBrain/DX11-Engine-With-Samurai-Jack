#pragma once

#include "IEditorObject.h"

EDITOR_BEGIN

class ISerializableCollider : public IEditorObject
{
protected:

	virtual void Awake() override;

	virtual Collider* GetCollider() const = 0;
	virtual void OnImGui() override;
	virtual bool Raycast(ICamera* camera, V3& out_hit) const override = 0;
	virtual string GetType() const override = 0;
	virtual void OnSerialize(Json::Value& json) const override;
	virtual void OnDeserialize(const Json::Value& json) override;
	virtual void OnDeserializeInRuntime(const Json::Value& json) override;
};

EDITOR_END
