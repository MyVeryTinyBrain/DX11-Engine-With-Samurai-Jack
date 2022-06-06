#pragma once

#include "EditorWindow.h"

EDITOR_BEGIN

class IEditorObject;

class Inspector : public EditorWindow
{
private:

	virtual void OnImGuiRender() override;

public:

	void Select(Ref<IEditorObject> object);
	Ref<IEditorObject> GetSelected() const { return m_object; }

private:

	Ref<IEditorObject> m_object;
};

EDITOR_END
