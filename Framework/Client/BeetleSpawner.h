#pragma once

#include "IEditorObject.h"
#include "EventListener.h"

class EnemyBeetleDrone;

class BeetleSpawner : public EDITOR::IEditorObject, public EventListener
{
private:

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void OnDestroyed() override;

	virtual void OnImGui() override;
	virtual bool Raycast(ICamera* camera, V3& out_hit) const override;
	virtual string GetType() const override;
	virtual void OnSerialize(Json::Value& json) const override;
	virtual void OnDeserialize(const Json::Value& json) override;
	virtual void OnDeserializeInRuntime(const Json::Value& json) override;
	virtual void OnEvent(const string& msg, void* pContext) override;

private:

	bool m_spawn = false;
	int m_numSpawn = 10;
	int m_numDead = 0;
	int m_numLeft;
	float m_spawnLeftTime = 0.0f;
	float m_spawnDuration = 3.0f;
	vector<Ref<EnemyBeetleDrone>> m_enemies;

	MeshRenderer* m_meshRenderer;
	BoxCollider* m_area;
};

