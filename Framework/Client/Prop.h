#pragma once

#include "IEditorObject.h"
#include "IEditorEventListener.h"

EDITOR_BEGIN

class Prop : public IEditorObject, public IEditorEventListener
{
private:

	virtual void Awake() override;
	virtual void OnDestroyed() override;

public:

	void SetMesh(const tstring& path);

private:

	virtual void OnImGui() override;
	virtual bool Raycast(ICamera* camera, V3& out_hit) const override;
	virtual string GetType() const override;
	virtual void OnSerialize(Json::Value& json) const override;
	virtual void OnDeserialize(const Json::Value& json) override;
	virtual void OnDeserializeInRuntime(const Json::Value& json) override;
	virtual void OnEvent(const string& msg, void* pContext) override;

private:

	void SetCollider(bool value);
	void SetMakeShadow(bool value);

private:

	GameObject* m_goMeshRenderer;
	MeshRenderer* m_meshRenderer;
	ConvexCollider* m_collider;
	bool m_makeShadow = true;
};

EDITOR_END
