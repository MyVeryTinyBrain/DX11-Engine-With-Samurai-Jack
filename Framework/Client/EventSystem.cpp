#include "stdafx.h"
#include "EventSystem.h"

EventSystem* EventSystem::g_eventSystem = nullptr;

void EventSystem::Awake()
{
	if (g_eventSystem)
	{
		Destroy();
	}

	g_eventSystem = this;
}

void EventSystem::OnDestroyed()
{
	if (g_eventSystem == this)
	{
		g_eventSystem = nullptr;
	}
}

void EventSystem::RegistListener(EventListener* listener)
{
	EventSystem* instance = g_eventSystem;
	if (!instance)
		return;

	auto find_it = std::find(instance->m_listeners.begin(), instance->m_listeners.end(), listener);
	if (find_it != instance->m_listeners.end())
		return;

	instance->m_listeners.push_back(listener);
}

void EventSystem::UnregistListener(EventListener* listener)
{
	EventSystem* instance = g_eventSystem;
	if (!instance)
		return;

	auto find_it = std::find(instance->m_listeners.begin(), instance->m_listeners.end(), listener);
	if (find_it != instance->m_listeners.end())
	{
		instance->m_listeners.erase(find_it);
	}
}

void EventSystem::Notify(const string msg, void* pContext)
{
	EventSystem* instance = g_eventSystem;
	if (!instance)
		return;

	for (auto& listener : instance->m_listeners)
	{
		listener->OnEvent(msg, pContext);
	}
}
