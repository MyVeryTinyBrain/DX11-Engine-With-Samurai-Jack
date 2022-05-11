#include "EnginePCH.h"
#include "AssimpData.h"
#include "VIType.h"
#include "NodeSet.h"
#include "Node.h"
#include "AnimationChannel.h"
#include "Animation.h"
#include "AnimationSet.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

AssimpData::AssimpData(
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
	bool hasAnimations)
{
	m_importer = importer;
	m_scene = scene;
	m_path = path;
	m_numVertices = numVertices;
	m_numSubMeshes = numSubMeshes;
	m_subMeshStartVertexIndices = subMeshStartVertexIndices;
	m_subMeshNames = subMeshNames;
	m_materials = materials;
	m_materialIndices = subMeshMaterialIndices;
	m_nodeDescsByName = nodeDescsByName;
	m_nodeDescsByIndex = nodeDescsByIndex;
	m_hasAnimations = hasAnimations;
}

AssimpData::~AssimpData()
{
	SafeDelete(m_importer);
}

AssimpData* AssimpData::CreateAssimpLoadedDataFromFile(const tstring& path, tstring& errorMessage)
{
	errorMessage = TEXT("");

	string strPath = tstring_to_string(path);

	// =================================================================================================

	Assimp::Importer* importer = new Assimp::Importer;
	if (!importer)
		return nullptr;

	auto ReleaseVars = [&]()
	{
		SafeDelete(importer);
	};

	importer->SetPropertyBool(AI_CONFIG_PP_DB_ALL_OR_NONE, true);
	importer->SetPropertyFloat(AI_CONFIG_PP_DB_THRESHOLD, 1.0f);
	
	const aiScene* scene = importer->ReadFile(
		strPath,
		aiProcess_ConvertToLeftHanded |
		aiProcess_Triangulate |
		aiProcess_CalcTangentSpace |

		aiProcess_GenSmoothNormals |
		aiProcess_FixInfacingNormals |

		aiProcess_JoinIdenticalVertices |
		aiProcess_LimitBoneWeights |
		aiProcess_OptimizeMeshes |

		aiProcess_Debone
	);

	if (!scene)
	{
		string strErrorMessage = importer->GetErrorString();
		tstring tstrErrorMessage = string_to_tstring(strErrorMessage);
		errorMessage = tstrErrorMessage;

		SafeDelete(importer);
		return nullptr;
	}

	// =================================================================================================

	uint numVertices = 0;
	vector<uint> subMeshStartVertexIndices;
	if (FAILED(SetupVertexCountAndSubMeshStartVertexIndices(scene, numVertices, subMeshStartVertexIndices)))
	{
		ReleaseVars();
		return nullptr;
	}

	vector<tstring> names;
	if (FAILED(SetupSubMeshNames(scene, names)))
	{
		ReleaseVars();
		return nullptr;
	}
	
	vector<ModelMaterialDesc> materials;
	if (FAILED(SetupMaterials(scene, path, materials)))
	{
		ReleaseVars();
		return nullptr;
	}

	vector<uint> subMeshMaterialIndices;
	if (FAILED(SetupSubMeshMaterialIndices(scene, subMeshMaterialIndices)))
	{
		ReleaseVars();
		return nullptr;
	}

	unordered_map<string, NodeDesc> nodeDescsByName;
	vector<NodeDesc> nodeDescsByIndex;
	if(FAILED(SetupNodeDescsByNameAndIndex(scene, nodeDescsByName, nodeDescsByIndex)))
	{
		ReleaseVars();
		return nullptr;
	}

	bool hasAnimation;
	if (FAILED(SetupAnimationContainsFlag(scene, hasAnimation)))
	{
		ReleaseVars();
		return nullptr;
	}

	// =================================================================================================

	return new AssimpData(
		importer, scene, 
		path, 
		numVertices, 
		scene->mNumMeshes,
		std::move(subMeshStartVertexIndices), 
		std::move(names),
		std::move(materials),
		std::move(subMeshMaterialIndices),
		std::move(nodeDescsByName),
		std::move(nodeDescsByIndex),
		hasAnimation);
}

inline uint AssimpData::GetNumIndexOfSubMesh(uint subMeshIndex)
{
	return m_scene->mMeshes[subMeshIndex]->mNumFaces * 3;
}

inline uint AssimpData::GetNumTriangleIndexOfSubMesh(uint subMeshIndex)
{
	return m_scene->mMeshes[subMeshIndex]->mNumFaces;
}

Vertex* AssimpData::CreateVertices() const
{
	uint temp;
	return CreateVertices(temp);
}

Vertex* AssimpData::CreateVertices(uint& out_count) const
{
	// 유니폼 초기화로 인하여 모든 값이 0으로 설정되었습니다.
	Vertex* vertices = new Vertex[m_numVertices]{};

	auto ReleaseVars = [&]()
	{
		SafeDeleteArray(vertices);
	};
	
	uint vertexCursor = 0;
	for (uint i = 0; i < scene->mNumMeshes; ++i)
	{
		// i: submesh index

		const aiMesh* mesh = scene->mMeshes[i];
		
		// 정점 배열에 값을 복사합니다.
		for (uint j = 0; j < mesh->mNumVertices; ++j)
		{
			memcpy(&vertices[vertexCursor].position, &mesh->mVertices[j], sizeof(aiVector3D));
			memcpy(&vertices[vertexCursor].normal, &mesh->mNormals[j], sizeof(aiVector3D));
			memcpy(&vertices[vertexCursor].uvw, &mesh->mTextureCoords[0][j], sizeof(aiVector3D));
			memcpy(&vertices[vertexCursor].tangent, &mesh->mTangents[j], sizeof(aiVector3D));
			memcpy(&vertices[vertexCursor].biNormal, &mesh->mBitangents[j], sizeof(aiVector3D));

			++vertexCursor;
		}

		// 정점 배열에 블렌드 값을 복사합니다.
		for (uint j = 0; j < mesh->mNumBones; ++j)
		{
			// j: node index

			const aiBone* bone = mesh->mBones[j];
			if (!bone)
			{
				ReleaseVars();
				return nullptr;
			}

			for (uint k = 0; k < bone->mNumWeights; ++k)
			{

				// weight.mVertexId 값은 서브메쉬의 로컬 인덱스입니다.
				// 만약 총 정점의 갯수가 100개인데, 어느 서브메쉬의 정점갯수가 10개라면
				// weight.mVertexId = [0 ~ 9] 입니다.
				// 따라서 서브메쉬의 시작 정점 인덱스를 더함으로써 실제 정점의 인덱스로 바꾸어줍니다.

				const aiVertexWeight& weight = bone->mWeights[k];
				uint blendedVertexIndex = weight.mVertexId + m_subMeshStartVertexIndices[i];

				// 최대 4개의 뼈로부터 영향을 받으므로
				// 이미 설정된 블렌드 가중치와 블렌드 인덱스는 무시합니다.

				// 블렌드 인덱스는 영향을 받을 뼈의 인덱스입니다.
				// aiMesh::bones 에 정의된 인덱스와 일치합니다.

				if (vertices[blendedVertexIndex].blendWeight.x == 0)
				{
					vertices[blendedVertexIndex].blendWeight.x = weight.mWeight;
					vertices[blendedVertexIndex].blendIndices.x = j;
				}
				else if (vertices[blendedVertexIndex].blendWeight.y == 0)
				{
					vertices[blendedVertexIndex].blendWeight.y = weight.mWeight;
					vertices[blendedVertexIndex].blendIndices.y = j;
				}
				else if (vertices[blendedVertexIndex].blendWeight.z == 0)
				{
					vertices[blendedVertexIndex].blendWeight.z = weight.mWeight;
					vertices[blendedVertexIndex].blendIndices.z = j;
				}
				else if (vertices[blendedVertexIndex].blendWeight.w == 0)
				{
					vertices[blendedVertexIndex].blendWeight.w = weight.mWeight;
					vertices[blendedVertexIndex].blendIndices.w = j;
				}
			}
		}
	}

	out_count = m_numVertices;
	return vertices;
}

TriangleIndex* AssimpData::CreateTriangleIndices(uint subMeshIndex) const
{
	uint temp;
	return CreateTriangleIndices(subMeshIndex, temp);
}

TriangleIndex* AssimpData::CreateTriangleIndices(uint subMeshIndex, uint& out_count) const
{
	out_count = 0;

	if (subMeshIndex >= m_scene->mNumMeshes)
		return nullptr;

	const aiMesh* mesh = m_scene->mMeshes[subMeshIndex];
	if (!mesh)
		return nullptr;
	
	Index startIndex = m_subMeshStartVertexIndices[subMeshIndex];

	TriangleIndex* triangles = new TriangleIndex[mesh->mNumFaces]{};
	for (uint i = 0; i < mesh->mNumFaces; ++i)
	{
		triangles[i]._0 = mesh->mFaces[i].mIndices[0] + startIndex;
		triangles[i]._1 = mesh->mFaces[i].mIndices[1] + startIndex;
		triangles[i]._2 = mesh->mFaces[i].mIndices[2] + startIndex;
	}

	out_count = mesh->mNumFaces;
	return triangles;
}

HRESULT AssimpData::CreateSubMeshNodeIndices(uint subMeshIndex, vector<uint>& out_nodeIndices)
{
	if (!m_scene)
		return E_FAIL;

	if (subMeshIndex >= m_scene->mNumMeshes)
		return E_FAIL;

	const aiMesh* mesh = m_scene->mMeshes[subMeshIndex];
	if (!mesh)
		return E_FAIL;

	for (uint j = 0; j < mesh->mNumBones; ++j)
	{
		const aiBone* bone = mesh->mBones[j];
		if (!bone)
			return E_FAIL;

		auto find_it = m_nodeDescsByName.find(bone->mName.data);

		//[!]
		// 이 경우 뼈가 존재하지 않는 노드입니다.
		// 뼈가 존재하지 않는 노드는 사용되지 않으므로 넘깁니다.
		if (find_it == m_nodeDescsByName.end())
			continue;
		//if (find_it == m_nodeDescsByName.end())
		//	return E_FAIL;

		const NodeDesc* nodeDesc = &find_it->second;

		uint nodeIndex = nodeDesc->index;
		out_nodeIndices.push_back(nodeIndex);
	}

	return S_OK;
}

vector<tstring> AssimpData::CreateSubMeshNames() const
{
	return m_subMeshNames;
}

const vector<ModelMaterialDesc>& AssimpData::CreateMaterialDescs() const
{
	return m_materials;
}

const vector<uint>& AssimpData::CreateMaterialIndices() const
{
	return m_materialIndices;
}

NodeSet* AssimpData::CreateNodeSet() const
{
	// 노드가 없다면 노드셋을 반환하지 않습니다.
	if (m_nodeDescsByIndex.empty())
		return nullptr;

	// 본 세트의 본 인덱스는 애니메이션이 사용하게 될 본 인덱스와 같습니다.

	vector<Node*> nodesByIndex;
	unordered_map<tstring, Node*> nodesByName;

	auto ReleaseVars = [&]()
	{
		for (auto& node : nodesByIndex)
		{
			SafeDelete(node);
		}
		nodesByIndex.clear();
		nodesByName.clear();
	};

	// const NodeDesc*: NodeDesc
	// Node*: Parent Node
	using Pair = pair<const NodeDesc*, Node*>;

	queue<Pair> q;
	q.push(make_pair(&m_nodeDescsByIndex[0], nullptr));
	
	while (!q.empty())
	{
		const Pair& pair = q.front();
		q.pop();

		const NodeDesc* nodeDesc = pair.first;
		Node* parent = pair.second;

		const aiNode* AINode = nodeDesc->AINode;
		if (!AINode)
		{
			ReleaseVars();
			return nullptr;
		}

		Node* node = new Node(nodeDesc->name, nodeDesc->hasBone, nodeDesc->offsetMatrix, nodeDesc->transformationMatrix, nodeDesc->index, parent, nodeDesc->depth);
		nodesByIndex.push_back(node);
		bool emplaceSucceeded = nodesByName.emplace(nodeDesc->name, node).second;

		if (!emplaceSucceeded)
		{
			ReleaseVars();
			return nullptr;
		}
		
		for (uint i = 0; i < AINode->mNumChildren; ++i)
		{
			const aiNode* child = AINode->mChildren[i];

			auto find_it = m_nodeDescsByName.find(child->mName.data);
			if (find_it == m_nodeDescsByName.end())
			{
				//[!]
				// 뼈가 존재하지 않는 노드는 미리 캐싱되어 있지 않습니다.
				// 따라서 존재하지 않는 노드라고 가정합니다.
				continue;

				//ReleaseVars();
				//return nullptr;
			}

			const NodeDesc* childNodeDesc = &find_it->second;
			q.push(make_pair(childNodeDesc, node));
		}
	}

	if(nodesByIndex.size() != nodesByName.size())
	{
		ReleaseVars();
		return nullptr;
	}

	NodeSet* nodeSet = new NodeSet(std::move(nodesByIndex), std::move(nodesByName));

	return nodeSet;
}

AnimationSet* AssimpData::CreateAnimationSet() const
{
	// Return empty Animtion set
	if (!m_scene->HasAnimations())
		return new AnimationSet();

	vector<AnimationChannel*> channels;
	vector<Animation*> animations;

	auto ReleaseVars = [&]()
	{
		for (auto& channel : channels)
		{
			SafeDelete(channel);
		}
		channels.clear();

		for (auto& animation : animations)
		{
			SafeDelete(animation);
		}
		animations.clear();
	};

	for (uint i = 0; i < m_scene->mNumAnimations; ++i)
	{
		const aiAnimation* AIAnimation = m_scene->mAnimations[i];
		if (!AIAnimation)
		{
			ReleaseVars();
			return nullptr;
		}

		vector<AnimationChannel*> temp;
		channels.swap(temp);
		channels.resize(m_nodeDescsByIndex.size());

		for (uint j = 0; j < AIAnimation->mNumChannels; ++j)
		{
			const aiNodeAnim* aiChannel = AIAnimation->mChannels[j];
			if (!aiChannel)
			{
				ReleaseVars();
				return nullptr;
			}
			
			auto find_it = m_nodeDescsByName.find(aiChannel->mNodeName.data);
			if (find_it == m_nodeDescsByName.end())
			{
				//[!]
				// 뼈가 존재하지 않는 노드에 대한 채널을 생성하지 않습니다.
				continue;
				//ReleaseVars();
				//return nullptr;
			}

			const NodeDesc* nodeDesc = &find_it->second;

			uint numKeyframe = Max(aiChannel->mNumPositionKeys, aiChannel->mNumRotationKeys);
			numKeyframe = Max(numKeyframe, aiChannel->mNumScalingKeys);

			vector<KeyframeDesc> keyframes;
			keyframes.reserve(numKeyframe);

			for (uint k = 0; k < numKeyframe; ++k)
			{
				KeyframeDesc keyframeDesc = {};
				
				if (k < aiChannel->mNumPositionKeys)
				{
					memcpy(&keyframeDesc.position, &aiChannel->mPositionKeys[k].mValue, sizeof(aiVector3D));
					keyframeDesc.time = float(aiChannel->mPositionKeys[k].mTime);
				}

				if (k < aiChannel->mNumRotationKeys)
				{
					keyframeDesc.rotation.x = aiChannel->mRotationKeys[k].mValue.x;
					keyframeDesc.rotation.y = aiChannel->mRotationKeys[k].mValue.y;
					keyframeDesc.rotation.z = aiChannel->mRotationKeys[k].mValue.z;
					keyframeDesc.rotation.w = aiChannel->mRotationKeys[k].mValue.w;
					keyframeDesc.time = float(aiChannel->mRotationKeys[k].mTime);
				}

				if (k < aiChannel->mNumScalingKeys)
				{
					memcpy(&keyframeDesc.scale, &aiChannel->mScalingKeys[k].mValue, sizeof(aiVector3D));
					keyframeDesc.time = float(aiChannel->mScalingKeys[k].mTime);
				}

				keyframes.push_back(keyframeDesc);
			}

			AnimationChannel* channel = new AnimationChannel(nodeDesc->name, nodeDesc->index, std::move(keyframes));
			channels[nodeDesc->index] = channel;
		}

		tstring animName = string_to_tstring(AIAnimation->mName.data);
		Animation* animation = new Animation(animName, (float)AIAnimation->mDuration, (float)AIAnimation->mTicksPerSecond, std::move(channels));
		animations.push_back(animation);
	}

	return new AnimationSet(std::move(animations));
}

bool AssimpData::HasAnimations() const
{
	return m_hasAnimations;
}

HRESULT AssimpData::SetupVertexCountAndSubMeshStartVertexIndices(const aiScene* scene, uint& out_vertexCount, vector<uint>& out_subMeshStartvertexIndices)
{
	// 정점의 총 갯수와 서브메쉬가 정점 배열에서 시작하게 되는 벡터를 반환합니다.
	// 서브메쉬는 정점의 특정 인덱스부터를 0번쨰 정점으로 인식하게 됩니다.

	if (!scene)
		return E_FAIL;

	out_vertexCount = 0;
	out_subMeshStartvertexIndices.clear();
	out_subMeshStartvertexIndices.reserve(scene->mNumMeshes);

	for (uint i = 0; i < scene->mNumMeshes; ++i)
	{
		const aiMesh* mesh = scene->mMeshes[i];
		if (!mesh)
			return E_FAIL;

		out_subMeshStartvertexIndices.push_back(out_vertexCount);
		out_vertexCount += mesh->mNumVertices;
	}

	return S_OK;
}

HRESULT AssimpData::SetupSubMeshNames(const aiScene* scene, vector<tstring>& out_subMeshNames)
{
	// 서브메쉬의 이름을 반환합니다.

	if (!scene)
		return E_FAIL;

	out_subMeshNames.clear();
	out_subMeshNames.reserve(scene->mNumMeshes);

	for (uint i = 0; i < scene->mNumMeshes; ++i)
	{
		const aiMesh* mesh = scene->mMeshes[i];
		if (!mesh)
			return E_FAIL;

		string strName = mesh->mName.data;
		tstring name = string_to_tstring(strName);
		out_subMeshNames.push_back(std::move(name));
	}

	return S_OK;
}

HRESULT AssimpData::SetupMaterials(const aiScene* scene, const tstring& path, vector<ModelMaterialDesc>& out_materials)
{
	// 이 모델에 사용되는 재질들을 반환합니다.
	// 서브메쉬는 이 재질의 인덱스를 참조합니다.

	if (!scene)
		return E_FAIL;

	out_materials.clear();
	out_materials.reserve(scene->mNumMaterials);

	fs::path directory = path;
	directory = directory.relative_path();
	directory = directory.parent_path();

	for (uint i = 0; i < scene->mNumMaterials; ++i)
	{
		aiMaterial* material = scene->mMaterials[i];
		if (!material)
			return E_FAIL;

		ModelMaterialDesc desc = {};

		// 해당 타입의 텍스쳐가 존재하면 문자열에 텍스쳐의 이름과 확장자(Filename.extension)을 복사합니다.
		// 해당 타입의 텍스쳐가 존재하면 1을 반환합니다.
		// 존재하지 않는다면 0을 반환합니다.
		auto StoreToDescFilename = [&](aiTextureType aiType, TCHAR* nameInDesc) -> uint
		{
			aiString aiStrPath;
			if (SUCCEEDED(material->GetTexture(aiType, 0, &aiStrPath)))
			{
				fs::path fspath = string_to_tstring(aiStrPath.data);
				fspath = directory / fspath.filename();
				tstring tstrPath = wstring_to_tstring(fspath.generic_wstring());
				std::transform(tstrPath.begin(), tstrPath.end(), tstrPath.begin(), ::tolower);
				_tcscpy_s(nameInDesc, MAX_PATH, tstrPath.c_str());
				return 1;
			}
			return 0;
		};

		desc.numSucceeded += StoreToDescFilename(aiTextureType_NONE, desc.none);
		desc.numSucceeded += StoreToDescFilename(aiTextureType_DIFFUSE, desc.diffuse);
		desc.numSucceeded += StoreToDescFilename(aiTextureType_SPECULAR, desc.specularMask);
		desc.numSucceeded += StoreToDescFilename(aiTextureType_AMBIENT, desc.ambient);
		desc.numSucceeded += StoreToDescFilename(aiTextureType_EMISSIVE, desc.emission);
		desc.numSucceeded += StoreToDescFilename(aiTextureType_HEIGHT, desc.height);
		desc.numSucceeded += StoreToDescFilename(aiTextureType_NORMALS, desc.normals);
		desc.numSucceeded += StoreToDescFilename(aiTextureType_SHININESS, desc.shininess);
		desc.numSucceeded += StoreToDescFilename(aiTextureType_OPACITY, desc.opacity);
		desc.numSucceeded += StoreToDescFilename(aiTextureType_DISPLACEMENT, desc.displacement);
		desc.numSucceeded += StoreToDescFilename(aiTextureType_LIGHTMAP, desc.lightmap);
		desc.numSucceeded += StoreToDescFilename(aiTextureType_REFLECTION, desc.reflection);
		desc.numSucceeded += StoreToDescFilename(aiTextureType_BASE_COLOR, desc.baseColor);
		desc.numSucceeded += StoreToDescFilename(aiTextureType_UNKNOWN, desc.unknown);

		out_materials.push_back(std::move(desc));
	}

	return S_OK;
}

HRESULT AssimpData::SetupSubMeshMaterialIndices(const aiScene* scene, vector<uint>& out_materialIndices)
{
	// 서브메쉬가 사용하는 재질의 인덱스가 담긴 벡터를 반환합니다.
	// GetMaterials() 함수로 얻어온 재질 벡터의 인덱스입니다.

	if (!scene)
		return E_FAIL;

	out_materialIndices.clear();
	out_materialIndices.reserve(scene->mNumMeshes);

	for (uint i = 0; i < scene->mNumMeshes; ++i)
	{
		const aiMesh* mesh = scene->mMeshes[i];
		if (!mesh)
			return E_FAIL;

		uint index = mesh->mMaterialIndex;
		if (index >= scene->mNumMaterials)
			return E_FAIL;

		out_materialIndices.push_back(index);
	}

	return S_OK;
}

HRESULT AssimpData::SetupNodeDescsByNameAndIndex(const aiScene* scene, unordered_map<string, NodeDesc>& out_nodeDescsByName, vector<NodeDesc>& out_nodeDescsByIndex)
{
	// 넓이 우선탐색 방식을 이용해 노드와 본을 재구성한 정보들을 이름과 인덱스로 매핑하여 반환합니다.
	// 인덱스로 매핑된 벡터의 첫 번째에는 루트 노드가 위치하게 됩니다.

	if (!scene)
		return E_FAIL;

	const aiNode* root = scene->mRootNode;
	if (!root)
		return E_FAIL;

	// Make bones map by name ==========================================================================

	unordered_map<string, const aiBone*> bonesByName;

	for (uint i = 0; i < scene->mNumMeshes; ++i)
	{
		const aiMesh* mesh = scene->mMeshes[i];
		if (!mesh)
			return E_FAIL;

		for (uint j = 0; j < mesh->mNumBones; ++j)
		{
			const aiBone* bone = mesh->mBones[j];
			if (!bone)
				return E_FAIL;

			bonesByName[bone->mName.data] = bone;
		}
	}

	// Make node descriptions ==========================================================================

	out_nodeDescsByName.clear();
	out_nodeDescsByIndex.clear();

	// const aiNode*: aiNode
	// bool: has parent
	// uint: parent index
	// uint: depth
	using Tuple = tuple<const aiNode*, bool, uint, uint>;

	queue<Tuple> q;
	q.push(make_tuple(root, false, UINT_MAX, 0));

	while (!q.empty())
	{
		const Tuple& front = q.front();
		q.pop();

		const aiNode* node = get<0>(front);
		bool hasParent = get<1>(front);
		uint parent = get<2>(front);
		uint depth = get<3>(front);
		uint index = uint(out_nodeDescsByIndex.size());

		NodeDesc nodeDesc = {};

		tstring tstrName = string_to_tstring(node->mName.data);
		_tcscpy_s(nodeDesc.name, tstrName.c_str());

		auto find_it = bonesByName.find(node->mName.data);
		if (find_it != bonesByName.end())
		{
			const aiBone* aiBone = find_it->second;
			memcpy(&nodeDesc.offsetMatrix, &aiBone->mOffsetMatrix, sizeof(aiMatrix4x4));
			nodeDesc.offsetMatrix.Transpose();
			nodeDesc.hasBone = true;
		}
		else
		{
			// [!]
			// 뼈가 존재하지 않는 노드는 무시합니다.
			for (uint i = 0; i < node->mNumChildren; ++i)
			{
				bool _hasParent = (depth > 0);
				q.push(make_tuple(node->mChildren[i], _hasParent, parent, depth));
			}
			continue;

			//nodeDesc.offsetMatrix = M4::identity();
			//nodeDesc.hasBone = false;
		}

		memcpy(&nodeDesc.transformationMatrix, &node->mTransformation, sizeof(aiMatrix4x4));
		nodeDesc.transformationMatrix.Transpose();

		nodeDesc.index = index;

		nodeDesc.hasParent = hasParent;

		nodeDesc.parent = parent;

		nodeDesc.depth = depth;

		nodeDesc.AINode = node;

		out_nodeDescsByName.emplace(node->mName.data, nodeDesc);
		out_nodeDescsByIndex.push_back(nodeDesc);

		for (uint i = 0; i < node->mNumChildren; ++i)
			q.push(make_tuple(node->mChildren[i], true, index, depth + 1));
	}

	// =================================================================================================

	return S_OK;
}

HRESULT AssimpData::SetupAnimationContainsFlag(const aiScene* scene, bool& out_result)
{
	// 애니메이션 보유 여부를 반환합니다.

	if (!scene)
		return E_FAIL;

	out_result = scene->HasAnimations();

	return S_OK;
}

