#pragma once

EDITOR_BEGIN

class IEditorObject;

class EditorDeserialize abstract
{
public:

	static IEditorObject* NewEditorObjectByType(Scene* scene, const Json::Value& json);
	static void DeserializeAllInRuntimeScene(Scene* scene, const Json::Value& root);
};

EDITOR_END
