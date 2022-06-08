#pragma once

EDITOR_BEGIN

class MeshSelector : public Component
{
	enum class Mode { Name, Path };

private:

	virtual void Awake() override;
	virtual void Update() override;
	virtual void OnEnable() override;

public:

	void Show();
	void Reset();
	ResourceRef<Mesh> GetSelected();

private:

	void FindAllMeshes();
	void NotifySelected(bool close);

private:

	vector<ResourceRef<Mesh>> m_meshes;
	ResourceRef<Mesh> m_selected;
	string m_strSearch;
	MeshSelector::Mode m_mode = MeshSelector::Mode::Name;
};

EDITOR_END
