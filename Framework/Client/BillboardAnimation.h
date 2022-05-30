#pragma once

class BillboardAnimation : public Component
{
private:

	virtual void Awake() override;
	virtual void Update() override;

public:

	void SetSliced(uint2 sliced);
	void SetIndex(uint2 index);
	void IncreaseIndex();
	void SetColor(const Color& color);
	void SetTexture(ResourceRef<Texture> texture);
	void SetMesh(ResourceRef<Mesh> mesh);
	void SetDelay(float value) { m_autoDelay = Max(0.0f, value); }
	BillboardRenderer* GetBillboardRenderer() const { return m_renderer; }

private:

	BillboardRenderer* m_renderer;
	uint2 m_sliced = {};
	uint2 m_index = {};
	bool m_autoPlay = true;
	float m_autoDelay = 0.025f;
	float m_leftToIncrease = 0.0f;

public:

	delegate<void()> OnLooped;

};

