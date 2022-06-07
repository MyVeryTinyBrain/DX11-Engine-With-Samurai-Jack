#pragma once

#include "IEditorObject.h"

EDITOR_BEGIN

class PrefabPlayer : public IEditorObject
{
	virtual void Awake() override;

	virtual void OnImGui() override;
	virtual bool Raycast(ICamera* camera, V3& out_hit) const override;
	virtual string GetType() const override;
	virtual void OnSerialize(Json::Value& json) const override;
	virtual void OnDeserialize(const Json::Value& json) override;
	virtual void OnDeserializeInRuntime(const Json::Value& json) override;

	MeshRenderer* m_meshRenderer;
};

EDITOR_END
