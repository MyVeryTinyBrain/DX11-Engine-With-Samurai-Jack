#pragma once

#include "Renderer.h"
#include "IRendererCullOp.h"

ENGINE_BEGIN

class ENGINE_API MeshRenderer : public Renderer, public IRendererCullOp
{

private:

	virtual void Render() override;

public:

	virtual bool CullTest(ICamera* camera) const override;

	virtual Bounds GetBounds() const override;

	virtual bool IsValid() const override;

	const ResourceRef<Mesh>& GetMesh() const { return m_mesh; }

	void SetMesh(const ResourceRef<Mesh>& mesh);

	_declspec(property(get = GetMesh, put = SetMesh)) const ResourceRef<Mesh>& mesh;

private:

	void SetupMaterialsToDefault(const ResourceRef<Mesh>& mesh);

private:

	uint				m_techniqueIndex = 0;

};

ENGINE_END
