#pragma once

#include "EventListener.h"

class EventSystem : public Component
{
private:

	virtual void Awake() override;
	virtual void OnDestroyed() override;

public:

	static void RegistListener(EventListener* listener);
	static void UnregistListener(EventListener* listener);
	static void Notify(const string msg, void* pContext);

private:

	static EventSystem* g_eventSystem;

	vector<EventListener*> m_listeners;
};

