#pragma once

#include "ISerializable.h"

EDITOR_BEGIN

class Editor;
class EditorObjects;

class IEditorObject : public Component, public ISerializable
{
protected:

	virtual void Awake() override;
	virtual void OnDestroyed() override;

public:

	void ImGuiRender();
	virtual void OnImGui() = 0;
	virtual bool Raycast(ICamera* camera, V3& out_hit) const = 0;
	virtual Json::Value Serialize() const override;
	virtual void Deserialize(const Json::Value& json) override;

public:

	virtual string GetType() const = 0;

protected:

	virtual void OnSerialize(Json::Value& json) const = 0;
	virtual void OnDeserialize(const Json::Value& json) = 0;

public:

	Editor* GetEditor() const;
	EditorObjects* GetEditorObjects() const;

	_declspec(property(get = GetEditor)) Editor* editor;
	_declspec(property(get = GetEditorObjects)) EditorObjects* editorObjects;
};

EDITOR_END
