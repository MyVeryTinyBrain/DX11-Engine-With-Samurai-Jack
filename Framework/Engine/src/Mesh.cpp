#include "EnginePCH.h"
#include "Mesh.h"
#include "VIBuffer.h"
#include "ResourceManagement.h"
#include "VI.h"
#include "AssimpData.h"
#include "System.h"
#include "GraphicSystem.h"
#include "NodeSet.h"
#include "AnimationSet.h"
#include "AnimationClip.h"

Mesh::Mesh(
	ResourceManagement* management, bool managed, const tstring& path,  
	VIBuffer* viBuffer) :
	ResourceObject(management, managed, path),
	m_viBuffer(viBuffer)
{
}

Mesh::~Mesh()
{
	SafeDelete(m_viBuffer);

	SafeDelete(m_nodeSet);
	SafeDelete(m_animationSet);

	for (auto& animationClip : m_animationClipsByIndex)
	{
		SafeDelete(animationClip);
	}
	m_animationClipsByIndex.clear();
	m_animationClipsByName.clear();
}

bool Mesh::IsValid() const
{
	if (!m_viBuffer)
		return false;

	return m_viBuffer->IsValid();
}

uint Mesh::GetSubMeshCount() const
{
	if (!m_viBuffer)
		return 0;

	VI* vi = m_viBuffer->GetVI();
	return vi->GetSubIndicesCount();
}

inline const SubResourceRef<AnimationClip> Mesh::GetAnimationClipByIndex(uint index)
{
	if (index >= m_animationClipsByIndex.size())
		return nullptr;

	return m_animationClipsByIndex[index];
}

inline const SubResourceRef<AnimationClip> Mesh::GetAnimationClipByName(const tstring& name)
{
	auto find_it = m_animationClipsByName.find(name);
	if (find_it == m_animationClipsByName.end())
		return nullptr;

	return find_it->second;
}

const Bounds& Mesh::GetBounds() const
{
	return m_viBuffer->GetVI()->GetBounds();
}

bool Mesh::RaycastInLocalSpace(V3& localHit, const Ray& localRay, bool in90Degrees) const
{
	if (!m_viBuffer)
		return false;
	return m_viBuffer->RaycastInLocalSpace(localHit, localRay, in90Degrees);
}

bool Mesh::RaycastInWorldSpace(V3& worldHit, const Ray& worldRay, const M4& localToWorldMatrix, bool in90Degrees) const
{
	if (!m_viBuffer)
		return false;
	return m_viBuffer->RaycastInWorldSpace(worldHit, worldRay, localToWorldMatrix, in90Degrees);
}

ResourceRef<Mesh> Mesh::CreateMeshM(ResourceManagement* management, const tstring& resourceKey, const VIBuffer* viBuffer)
{
	if (!management)
		return nullptr;

	ResourceRef<ResourceObject> find = management->Find(resourceKey);
	if (find)
		return find;

	if(!viBuffer || !viBuffer->IsValid())
		return nullptr;

	return new Mesh(management, true, resourceKey, viBuffer->Copy());
}

ResourceRef<Mesh> Mesh::CreateMeshUM(ResourceManagement* management, const VIBuffer* viBuffer)
{
	if (!management)
		return nullptr;

	if (!viBuffer || !viBuffer->IsValid())
		return nullptr;

	return new Mesh(management, false, TEXT(""), viBuffer->Copy());
}

ResourceRef<Mesh> Mesh::CreateMeshNocopyM(ResourceManagement* management, const tstring& resourceKey, VIBuffer** ppVIBuffer)
{
	auto ReleaseVars = [&]()
	{
		SafeDelete(*ppVIBuffer);
	};

	if (!ppVIBuffer || !*ppVIBuffer)
		return nullptr;

	if (!management)
	{
		ReleaseVars();
		return nullptr;
	}

	ResourceRef<ResourceObject> find = management->Find(resourceKey);
	if (find)
		return find;

	if (!(*ppVIBuffer)->IsValid())
	{
		ReleaseVars();
		return nullptr;
	}

	Mesh* mesh = new Mesh(management, true, resourceKey, *ppVIBuffer);

	*ppVIBuffer = nullptr;

	return mesh;
}

ResourceRef<Mesh> Mesh::CreateMeshNocopyUM(ResourceManagement* management, VIBuffer** ppViBuffer)
{
	auto ReleaseVars = [&]()
	{
		SafeDelete(*ppViBuffer);
	};

	if (!ppViBuffer || !*ppViBuffer)
		return nullptr;

	if (!management)
	{
		ReleaseVars();
		return nullptr;
	}

	if (!(*ppViBuffer)->IsValid())
	{
		ReleaseVars();
		return nullptr;
	}

	Mesh* mesh = new Mesh(management, false, TEXT(""), *ppViBuffer);

	*ppViBuffer = nullptr;

	return mesh;
}

ResourceRef<Mesh> Mesh::LoadMeshM(ResourceManagement* management, const tstring& path)
{
	tstring errorMessage;
	AssimpData* data = nullptr;
	VI* viLoaded = nullptr;
	VIBuffer* viBufferLoaded = nullptr;

	auto ReleaseVars = [&]()
	{
		SafeDelete(data);
		SafeDelete(viLoaded);
		SafeDelete(viBufferLoaded);
	};

	if (!management)
	{
		ReleaseVars();
		ERROR_MESSAGE_NT((TEXT("Failed to load mesh: ") + path).c_str());
		return nullptr;
	}

	ResourceRef<ResourceObject> find = management->Find(path);
	if (find)
		return find;

	data = AssimpData::CreateAssimpLoadedDataFromFile(path, errorMessage);

	if (!data)
	{
		ReleaseVars();
		ERROR_MESSAGE_NT((TEXT("Failed to load mesh: ") + path).c_str());
		return nullptr;
	}

	viLoaded = new VI;
	Vertex* vertices = data->CreateVertices();
	viLoaded->SetVerticesNocopy(&vertices, data->GetNumVertices());
	viLoaded->SetSubIndicesCount(data->GetNumSubMeshes());
	for (uint i = 0; i < data->GetNumSubMeshes(); ++i)
	{
		TriangleIndex* triangles = data->CreateTriangleIndices(i);
		viLoaded->SetSubIndicesAsTrianglesNocopy(&triangles, data->GetNumTriangleIndexOfSubMesh(i), i);
	}

	viBufferLoaded = nullptr;
	VIBuffer::CreateVIBufferNocopy(
		management->system->graphic->device, management->system->graphic->deviceContext,
		&viLoaded,
		D3D11_USAGE_IMMUTABLE, 0, 0,
		D3D11_USAGE_IMMUTABLE, 0, 0,
		&viBufferLoaded);

	if (!viBufferLoaded->IsValid())
	{
		ReleaseVars();
		ERROR_MESSAGE_NT((TEXT("Failed to load mesh: ") + path).c_str());
		return nullptr;
	}

	Mesh* mesh = new Mesh(management, true, path, viBufferLoaded);

	mesh->m_subMeshNodeIndices.resize(data->numSubMeshes);
	for (uint i = 0; i < data->numSubMeshes; ++i)
	{
		if (FAILED(data->CreateSubMeshNodeIndices(i, mesh->m_subMeshNodeIndices[i])))
		{
			ReleaseVars();
			ERROR_MESSAGE_NT((TEXT("Failed to load mesh: ") + path).c_str());
			return nullptr;
		}
	}

	mesh->m_subMeshNames = data->CreateSubMeshNames();
	mesh->m_materialDescs = data->CreateMaterialDescs();
	mesh->m_materialIndices = data->CreateMaterialIndices();
	mesh->m_nodeSet = data->CreateNodeSet();
	mesh->m_animationSet = data->CreateAnimationSet();
	mesh->m_hasAnimations = data->HasAnimations();

	if (mesh->m_hasAnimations)
	{
		mesh->m_animationClipsByIndex.reserve(mesh->m_animationSet->animationCount);
		mesh->m_animationClipsByName.reserve(mesh->m_animationSet->animationCount);
		for (uint i = 0; i < mesh->m_animationSet->animationCount; ++i)
		{
			AnimationClip* clip = new AnimationClip(mesh, mesh->m_animationSet->GetAnimationByIndex(i));
			mesh->m_animationClipsByIndex.push_back(clip);
			mesh->m_animationClipsByName.emplace(clip->name, clip);
		}
	}

	SafeDelete(data);

	return mesh;
}

HRESULT Mesh::ApplyVertexBuffer(Com<ID3D11DeviceContext> deviceContext)
{
	return m_viBuffer->ApplyVertexBuffer(deviceContext);
}

HRESULT Mesh::ApplyIndexBuffer(Com<ID3D11DeviceContext> deviceContext)
{
	return m_viBuffer->ApplyIndexBuffer(deviceContext);
}

HRESULT Mesh::DrawSubMesh(Com<ID3D11DeviceContext> deviceContext, uint subMeshIndex)
{
	return m_viBuffer->DrawSubMesh(deviceContext, UINT(subMeshIndex));
}

HRESULT Mesh::DrawSubMesh(Com<ID3D11DeviceContext> deviceContext, uint subMeshIndex, uint primitiveCount)
{
	return m_viBuffer->DrawSubMesh(deviceContext, UINT(subMeshIndex), primitiveCount);
}

HRESULT Mesh::ApplyVertexAndInstanceBuffer(Com<ID3D11DeviceContext> deviceContext, Com<ID3D11Buffer> instanceBuffer)
{
	return m_viBuffer->ApplyVertexAndInstanceBuffer(deviceContext, instanceBuffer);
}

HRESULT Mesh::DrawInstanceSubMesh(Com<ID3D11DeviceContext> deviceContext, uint subMeshIndex, uint instanceCount)
{
	return m_viBuffer->DrawInstanceSubMesh(deviceContext, UINT(subMeshIndex), instanceCount);
}

HRESULT Mesh::DrawInstanceSubMesh(Com<ID3D11DeviceContext> deviceContext, uint subMeshIndex, uint instanceCount, uint primitiveCount)
{
	return m_viBuffer->DrawInstanceSubMesh(deviceContext, UINT(subMeshIndex), instanceCount, primitiveCount);
}
