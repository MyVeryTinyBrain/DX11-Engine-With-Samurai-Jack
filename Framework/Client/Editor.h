#pragma once

class FreeCamera;

EDITOR_BEGIN

class EditorWindow;
class EditorObjects;
class Spawner;
class Inspector;
class MeshSelector;
class EditorEventSystem;
class Hierarchy;

class Editor : public Component
{
	virtual void Awake() override;
	virtual void OnDestroyed() override;
	virtual void Update() override;

private:

	void Init();

public:

	void Serialize(const tstring& path);
	void Deserialize(const tstring& path);

public:

	Gizmo* GetGizmo() const { return m_gizmo; }
	FreeCamera* GetFreeCamera() const { return m_freeCamera; }
	EditorObjects* GetEditorObjects() const { return m_editorObjects; }
	Spawner* GetPropPlacer() const { return m_spawner; }
	Inspector* GetInspector() const { return m_inspector; }
	MeshSelector* GetMeshSelector() const { return m_meshSelector; }
	EditorEventSystem* GetEditorEventSystem() const { return m_editorEventSystem; }

public:

	bool OpenFileDialog(tstring& out_path);
	bool SaveFileDialog(tstring& out_path);

	void New();
	void Load();
	void Save();
	void SaveAs();

private:

	Gizmo* m_gizmo;
	FreeCamera* m_freeCamera;
	EditorObjects* m_editorObjects;
	Spawner* m_spawner;
	Inspector* m_inspector;
	MeshSelector* m_meshSelector;
	EditorEventSystem* m_editorEventSystem;
	Hierarchy* m_hierarchy;

	vector<EditorWindow*> m_editorWindows;

	tstring m_path;

private:

	static Editor* g_editor;

public:

	static Editor* GetInstance() { return g_editor; }
	static bool IsEditorMode() { return g_editor != nullptr; }
};

EDITOR_END
