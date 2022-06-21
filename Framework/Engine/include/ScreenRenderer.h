#pragma once

#include "Renderer.h"

ENGINE_BEGIN

class ENGINE_API ScreenRenderer : public Renderer
{
private:

	virtual void Awake() override;
	virtual void Render() override;

public:

	virtual Bounds GetBounds() const override;
	virtual bool IsValid() const override;

	const ResourceRef<Mesh>& GetMesh() const { return m_mesh; }
	void SetMesh(const ResourceRef<Mesh>& mesh);

	int GetRenderGroupOrder() const { return m_renderGroupOrder; }
	void SetRenderGroupOrder(int value) { m_renderGroupOrder = value; }

	_declspec(property(get = GetMesh, put = SetMesh)) const ResourceRef<Mesh>& mesh;
	_declspec(property(get = GetRenderGroupOrder, put = SetRenderGroupOrder)) int renderGroupOrder;

public:

	// ResourceRef<Mesh> before, ResourceRef<Mesh> after
	delegate<void(ResourceRef<Mesh>, ResourceRef<Mesh>)> OnMeshChanged;

	int m_renderGroupOrder = 0;
};

ENGINE_END
