#pragma once

EDITOR_BEGIN

class MeshSelector : public Component
{
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
};

EDITOR_END
