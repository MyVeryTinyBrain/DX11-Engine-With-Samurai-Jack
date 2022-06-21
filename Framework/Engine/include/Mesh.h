#pragma once

#include "ResourceObject.h"
#include "IMesh.h"
#include "ModelMaterialDesc.h"
#include "SubResourceRef.h"

ENGINE_BEGIN
class VIBuffer;
class NodeSet;
class AnimationSet;
class AnimationClip;
class ENGINE_API Mesh : public ResourceObject, public IMesh
{
protected:

	Mesh(
		ResourceManagement* management, bool managed, const tstring& path,
		VIBuffer* viBuffer);

	virtual ~Mesh();

public:

	bool IsValid() const;

	inline VIBuffer* GetVIBuffer() const { return m_viBuffer; }

	uint GetSubMeshCount() const;

	inline const vector<uint>& GetSubMeshNodeIndices(uint index) const { return m_subMeshNodeIndices[index]; }

	inline const tstring& GetSubMeshName(uint index) const { return m_subMeshNames[index]; }

	inline const vector<ModelMaterialDesc>& GetMaterialDescs() const { return m_materialDescs; }

	inline const vector<uint>& GetMaterialIndices() const { return m_materialIndices; }

	inline bool HasNodeSet() const { return m_nodeSet != nullptr; }

	inline const NodeSet* GetNodeSet() const { return m_nodeSet; }

	inline const AnimationSet* GetAnimationSet() const { return m_animationSet; }

	inline uint GetAnimationCount() const { return uint(m_animationClipsByIndex.size()); }

	inline const SubResourceRef<AnimationClip> GetAnimationClipByIndex(uint index);

	inline const SubResourceRef<AnimationClip> GetAnimationClipByName(const tstring& name);

	inline bool HasAnimations() const { return m_hasAnimations; }

	_declspec(property(get = IsValid)) bool isValid;
	_declspec(property(get = GetVIBuffer)) VIBuffer* viBuffer;
	_declspec(property(get = GetSubMeshCount)) uint subMeshCount;
	_declspec(property(get = GetMaterialDescs)) const vector<ModelMaterialDesc>& materialDescs;
	_declspec(property(get = GetMaterialIndices)) const vector<uint>& materialIndices;
	_declspec(property(get = HasNodeSet)) bool hasNodeSet;
	_declspec(property(get = GetNodeSet)) const NodeSet* nodeSet;
	_declspec(property(get = GetAnimationSet)) const AnimationSet* animtionSet;
	_declspec(property(get = HasAnimations)) bool hasAnimations;

public:

	const Bounds& GetBounds() const;

	bool RaycastInLocalSpace(V3& localHit, const Ray& localRay, bool in90Degrees = true) const;

	bool RaycastInWorldSpace(V3& worldHit, const Ray& worldRay, const M4& localToWorldMatrix, bool in90Degrees = true) const;

public:

	// VIBuffer is copy on other memory
	// you can release VIBuffer

	static ResourceRef<Mesh> CreateMeshM(ResourceManagement* management, const tstring& resourceKey, const VIBuffer* viBuffer);
	static ResourceRef<Mesh> CreateMeshUM(ResourceManagement* management, const VIBuffer* viBuffer);
	static ResourceRef<Mesh> CreateMeshNocopyM(ResourceManagement* management, const tstring& resourceKey, VIBuffer** ppVIBuffer);
	static ResourceRef<Mesh> CreateMeshNocopyUM(ResourceManagement* management, VIBuffer** ppVIBuffer);
	static ResourceRef<Mesh> LoadMeshM(ResourceManagement* management, const tstring& path);

private: 

	virtual HRESULT ApplyVertexBuffer(Com<ID3D11DeviceContext> deviceContext) override;

	virtual HRESULT ApplyIndexBuffer(Com<ID3D11DeviceContext> deviceContext) override;

	virtual HRESULT DrawSubMesh(Com<ID3D11DeviceContext> deviceContext, uint subMeshIndex) override;
	virtual HRESULT DrawSubMesh(Com<ID3D11DeviceContext> deviceContext, uint subMeshIndex, uint primitiveCount) override;

	virtual HRESULT ApplyVertexAndInstanceBuffer(Com<ID3D11DeviceContext> deviceContext, Com<ID3D11Buffer> instanceBuffer) override;

	virtual HRESULT DrawInstanceSubMesh(Com<ID3D11DeviceContext> deviceContext, uint subMeshIndex, uint instanceCount) override;
	virtual HRESULT DrawInstanceSubMesh(Com<ID3D11DeviceContext> deviceContext, uint subMeshIndex, uint instanceCount, uint primitiveCount) override;

private:

	VIBuffer*										m_viBuffer = nullptr;

	vector<vector<uint>>							m_subMeshNodeIndices;

	vector<tstring>									m_subMeshNames;

	vector<ModelMaterialDesc>						m_materialDescs;

	vector<uint>									m_materialIndices;

	NodeSet*										m_nodeSet = nullptr;

	AnimationSet*									m_animationSet = nullptr;

	vector<AnimationClip*>							m_animationClipsByIndex;

	unordered_map<tstring, AnimationClip*>			m_animationClipsByName;

	bool											m_hasAnimations = false;
};

ENGINE_END
