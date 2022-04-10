#pragma once

class PlayerTestScene : public Scene
{
	virtual Scene* Clone() override;
	virtual void OnLoad() override;
	virtual void OnUnload() override;
	virtual void OnUpdate() override;
	virtual void OnLateUpdate() override;

	GameObject* m_sphere[2] = {};
	TrailRenderer* m_trailRenderer[2] = {};
	Rigidbody* m_rigidbody = nullptr;
};

