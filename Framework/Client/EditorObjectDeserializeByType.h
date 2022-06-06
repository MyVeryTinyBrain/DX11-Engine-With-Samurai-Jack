#pragma once

EDITOR_BEGIN

class IEditorObject;

class EditorObjectDeserializeByType abstract
{
public:

	static IEditorObject* DeserializeByType(Scene* scene, const Json::Value& json);
};

EDITOR_END
