#pragma once

#include "IEditorObject.h"

EDITOR_BEGIN

class ISerializableLight : public IEditorObject
{
protected:

	virtual void Awake() override;

protected:

	virtual Light* InitLight() = 0;
	virtual void OnImGui() override;
	virtual bool Raycast(ICamera* camera, V3& out_hit) const override;
	virtual string GetType() const override = 0;
	virtual void OnSerialize(Json::Value& json) const override;
	virtual void OnDeserialize(const Json::Value& json) override;

private:

	MeshRenderer* m_meshRenderer;
	Light* m_light;
};

EDITOR_END
