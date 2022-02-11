#pragma once

#include "ModelMaterialDesc.h"
#include "NodeDesc.h"
#include "KeyframeDesc.h"

ENGINE_BEGIN
struct Vertex;
struct TriangleIndex;
class NodeSet;
class AnimationSet;
class ENGINE_API AssimpData
{
private:

	AssimpData(
		Assimp::Importer* importer,
		const aiScene* scene,
		const tstring& path,
		uint numVertices,
		uint numSubMeshes,
		vector<uint>&& subMeshStartVertexIndices,
		vector<tstring>&& subMeshNames,
		vector<ModelMaterialDesc>&& materials,
		vector<uint>&& subMeshMaterialIndices,
		unordered_map<string, NodeDesc>&& nodeDescsByName,
		vector<NodeDesc>&& nodeDescsByIndex,
		bool hasAnimations);

	AssimpData(const AssimpData& rhs) = delete;

	AssimpData(AssimpData&& rhs) = delete;

	AssimpData& operator = (const AssimpData& rhs) = delete;

	AssimpData& operator = (AssimpData&& rhs) = delete;

public:

	~AssimpData();

public:

	static AssimpData* CreateAssimpLoadedDataFromFile(const tstring& path, tstring& errorMessage);

	inline const Assimp::Importer* GetImporter() const { return m_importer; }

	inline const aiScene* GetScene() const { return m_scene; }

	inline const tstring& GetPath() const { return m_path; }

	inline uint GetNumVertices() const { return m_numVertices; }

	inline uint GetNumSubMeshes() const { return m_numSubMeshes; }

	inline uint GetNumIndexOfSubMesh(uint subMeshIndex);

	inline uint GetNumTriangleIndexOfSubMesh(uint subMeshIndex);

	_declspec(property(get = GetImporter)) const Assimp::Importer* importer;
	_declspec(property(get = GetScene)) const aiScene* scene;
	_declspec(property(get = GetPath)) const tstring& path;
	_declspec(property(get = GetNumVertices)) uint numVertices;
	_declspec(property(get = GetNumSubMeshes)) uint numSubMeshes;

public:

	Vertex* CreateVertices() const;

	Vertex* CreateVertices(uint& out_count) const;

	TriangleIndex* CreateTriangleIndices(uint subMeshIndex) const;

	TriangleIndex* CreateTriangleIndices(uint subMeshIndex, uint& out_count) const;

	HRESULT CreateSubMeshNodeIndices(uint subMeshIndex, vector<uint>& out_nodeIndices);

	vector<tstring> CreateSubMeshNames() const;

	const vector<ModelMaterialDesc>& CreateMaterialDescs() const;

	const vector<uint>& CreateMaterialIndices() const;

	NodeSet* CreateNodeSet() const;

	AnimationSet* CreateAnimationSet() const;

	bool HasAnimations() const;

private:

	static HRESULT SetupVertexCountAndSubMeshStartVertexIndices(const aiScene* scene, uint& out_vertexCount, vector<uint>& out_subMeshStartvertexIndices);

	static HRESULT SetupSubMeshNames(const aiScene* scene, vector<tstring>& out_subMeshNames);

	static HRESULT SetupMaterials(const aiScene* scene, const tstring& path, vector<ModelMaterialDesc>& out_materials);

	static HRESULT SetupSubMeshMaterialIndices(const aiScene* scene, vector<uint>& out_materialIndices);

	static HRESULT SetupNodeDescsByNameAndIndex(const aiScene* scene, unordered_map<string, NodeDesc>& out_nodeDescsByName, vector<NodeDesc>& out_nodeDescsByIndex);

	static HRESULT SetupAnimationContainsFlag(const aiScene* scene, bool& out_result);

private:

	// The imported data(aiScene) will be destroyed along with the Importer object
	Assimp::Importer*					m_importer;

	// You shouldn't want to instance it, nor should you ever try to delete a given scene on your own.
	const aiScene*						m_scene;

	tstring								m_path;

	// Vertex Informations...

	uint								m_numVertices = 0;

	// SubMesh Informations...

	uint								m_numSubMeshes = 0;

	vector<uint>						m_subMeshStartVertexIndices;

	vector<tstring>						m_subMeshNames;

	vector<ModelMaterialDesc>			m_materials;

	vector<uint>						m_materialIndices;

	// Node Informations ...

	unordered_map<string, NodeDesc>		m_nodeDescsByName;

	vector<NodeDesc>					m_nodeDescsByIndex;

	bool								m_hasAnimations;

};

ENGINE_END
