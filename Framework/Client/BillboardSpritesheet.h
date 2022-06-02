#pragma once

class BillboardSpritesheet : public Component
{
private:

	virtual void Awake() override;
	virtual void Update() override;

public:

	void SetSliced(uint2 sliced);
	void SetIndex(uint2 index);
	void SetColor(const Color& color);
	void SetTexture(ResourceRef<Texture> texture);
	void SetMesh(ResourceRef<Mesh> mesh);
	BillboardRenderer* GetBillboardRenderer() const { return m_renderer; }

private:

	BillboardRenderer* m_renderer;
	uint2 m_sliced = {};
	uint2 m_index = {};
};

