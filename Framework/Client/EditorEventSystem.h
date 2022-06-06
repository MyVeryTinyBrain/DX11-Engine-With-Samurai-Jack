#pragma once

EDITOR_BEGIN

class IEditorEventListener;

class EditorEventSystem : public Component
{
public:

	void Regist(IEditorEventListener* listener);
	void Unregist(IEditorEventListener* listener);
	void Notify(const string& msg, void* pContext);

private:

	vector<IEditorEventListener*> m_listenres;
};

EDITOR_END
