#pragma once

#include "EventListener.h"

class SceneAshi : public Scene, public EventListener
{
	virtual Scene* Clone() override;
	virtual void OnLoad() override;
	virtual void OnUnload() override;
	virtual void OnUpdate() override;
	virtual void OnLateUpdate() override;

	virtual void OnEvent(const string& msg, void* pContext) override;

private:

	vector<Ref<Collider>> m_fightColliders;
	vector<Ref<Collider>> m_nextSceneTriggers;
};

