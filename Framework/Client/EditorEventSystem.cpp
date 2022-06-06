#include "stdafx.h"
#include "EditorEventSystem.h"
#include "IEditorEventListener.h"

EDITOR_USE

void EditorEventSystem::Regist(IEditorEventListener* listener)
{
	auto find_it = std::find(m_listenres.begin(), m_listenres.end(), listener);
	if (find_it != m_listenres.end())
		return;

	m_listenres.push_back(listener);
}

void EditorEventSystem::Unregist(IEditorEventListener* listener)
{
	auto find_it = std::find(m_listenres.begin(), m_listenres.end(), listener);
	if (find_it != m_listenres.end())
	{
		m_listenres.erase(find_it);
	}
}

void EditorEventSystem::Notify(const string& msg, void* pContext)
{
	for (auto& listener : m_listenres)
	{
		listener->OnEvent(msg, pContext);
	}
}
