#pragma once

EDITOR_BEGIN

class EditorWindow : public Component
{
public:

	virtual void OnImGuiRender() = 0;
};

EDITOR_END
