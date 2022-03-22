#pragma once

#include "Renderer.h"
#include "IRendererBoneOp.h"
#include "IRendererCullOp.h"
#include "Ref.h"

ENGINE_BEGIN
class NodeTransform;
class ENGINE_API SkinnedMeshRenderer : public Renderer, public IRendererBoneOp, public IRendererCullOp
{

private:

	virtual void Render() override;

	virtual void OnDestroyed() override;

public:

	virtual bool CullTest(ICamera* camera) const override;

	virtual Bounds GetBounds() const override;

	virtual bool IsValid() const override;

	const ResourceRef<Mesh>& GetMesh() const { return m_mesh; }

	void SetMesh(const ResourceRef<Mesh>& mesh); 

	uint GetNodeTransformCount() const;

	Ref<NodeTransform> GetNodeTransformByIndex(uint index) const;

	Ref<NodeTransform> GetNodeTransformByName(const tstring& name) const;

	_declspec(property(get = GetMesh, put = SetMesh)) const ResourceRef<Mesh>& mesh;

private:

	virtual void OnSetBoneMatricesCBuffer(uint subMeshIndex, BoneMatricesCBuffer* pCBuffer) override;

private:

	void SetupMaterialsToDefault(const ResourceRef<Mesh>& mesh);

	void DeleteNodeTransforms();

	void SetupNodeTransforms();

private:

	NodeTransform*								m_rootNodeTransform = nullptr;

	vector<NodeTransform*>						m_nodeTransformsByIndex;

	unordered_map<tstring, NodeTransform*>		m_nodeTransformsByName;
};

ENGINE_END
