#pragma once

#include "IEditorObject.h"

class DirtGround : public EDITOR::IEditorObject
{
private:

	virtual void Awake() override;
	virtual void Update() override;

	virtual void OnImGui() override;
	virtual bool Raycast(ICamera* camera, V3& out_hit) const override;
	virtual string GetType() const override;
	virtual void OnSerialize(Json::Value& json) const override;
	virtual void OnDeserialize(const Json::Value& json) override;
	virtual void OnDeserializeInRuntime(const Json::Value& json) override;

private:

	MeshRenderer* m_meshRenderer;
};

