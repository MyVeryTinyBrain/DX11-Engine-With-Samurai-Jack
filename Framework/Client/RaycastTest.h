#pragma once

class RaycastTest : public Component
{
private:

	virtual void Awake() override;
	virtual void Update() override;

private:

	MeshRenderer* m_meshRenderer;
};

