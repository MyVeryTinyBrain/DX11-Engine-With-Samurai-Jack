#pragma once

EDITOR_BEGIN

class IEditorObject;

class EditorObjects : public Component
{
private:

	virtual void Update() override;

public:

	uint GetNumObjects() const { return (uint)m_objects.size(); }
	IEditorObject* GetObjectByIndex(uint index) const { return m_objects[index]; }

	void RegistObject(IEditorObject* object);
	void UnregistObject(IEditorObject* object);
	void ClearObjects();

	void Serialize(Json::Value& root) const;
	void Deserialize(const Json::Value& root);

	string GetNumberedName(const string& name) const;

private:

	vector<IEditorObject*> m_objects;
};

EDITOR_END

