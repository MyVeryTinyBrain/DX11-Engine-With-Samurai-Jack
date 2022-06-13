#pragma once

#include "IEditorObject.h"

class FlameVent : public EDITOR::IEditorObject
{
private:

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;

	virtual void OnImGui() override;
	virtual bool Raycast(ICamera* camera, V3& out_hit) const override;
	virtual string GetType() const override;
	virtual void OnSerialize(Json::Value& json) const override;
	virtual void OnDeserialize(const Json::Value& json) override;
	virtual void OnDeserializeInRuntime(const Json::Value& json) override;

private:

	MeshRenderer* m_meshRenderer;
	BoxCollider* m_area;
	PointLight* m_light;
	AudioSource* m_audioSource;

	float m_flameDuration = 0.5f;
	float m_flameLength = 10.0f;
	float m_delay = 4.0f;
	float m_throwDuration = 3.0f;
	bool m_throw = false;

	float m_waitTime = 0.0f;
	float m_throwTime = 0.0f;
};

