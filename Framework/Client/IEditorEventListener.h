#pragma once

EDITOR_BEGIN

class IEditorEventListener abstract
{
public:

	virtual void OnEvent(const string& msg, void* pContext) = 0;
};

EDITOR_END
