#include "EnginePCH.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include "TechniqueDesc.h"
#include "PassDesc.h"
#include "BuiltInResources.h"
#include "System.h"
#include "Times.h"
#include "Texture2D.h"
#include "ResourceFactory.h"
#include "ShaderVariable.h"
#include "ShaderVariableInfo.h"
#include "SpecificShaderVariables.h"
#include "ResourceManagement.h"
#include "JsonUtility.h"

Material::Material(ResourceManagement* management, bool managed, const tstring& path, ResourceRef<Shader> shader) :
	ResourceObject(management, managed, path)
{
	m_shader = shader;

	if (m_shader)
		SetupShaderVariables();
}

Material::Material(ResourceManagement* management, bool managed, const tstring& path, const Material& other) :
	ResourceObject(management, managed, path)
{
	m_shader = other.m_shader;
	m_techniqueIndex = other.m_techniqueIndex;

	if (m_shader)
	{
		for (auto& specificVariable : other.m_specificShaderVariables)
			m_specificShaderVariables.emplace_back(specificVariable->Copy(this, *specificVariable));

		for (auto& variable : other.m_shaderVariables)
			m_shaderVariables.emplace_back(variable->Copy());
	}
}

Material::~Material()
{
	for (auto& var : m_shaderVariables)
		SafeDelete(var);
	m_shaderVariables.clear();

	for (auto& var : m_specificShaderVariables)
		SafeDelete(var);
	m_specificShaderVariables.clear();
}

void Material::ApplyMaterial(Com<ID3D11DeviceContext> deviceContext, ICamera* camera)
{
	ApplyVariables();
	ApplySpecificVariables(deviceContext, camera);
}

HRESULT Material::SetRawValue(const string& name, const void* data, size_t size)
{
	if (!m_shader)
		return E_FAIL;

	auto find_it = std::find_if(m_shaderVariables.begin(), m_shaderVariables.end(), [&](const ShaderVariable* var)
		{
			return var->info->Name == name;
		});

	if (find_it == m_shaderVariables.end())
		return E_FAIL;

	(*find_it)->SetRawValue(data, size);

	return S_OK;
}

HRESULT Material::SetFloat(const string& name, float value)
{
	if (!m_shader)
		return E_FAIL;

	auto find_it = std::find_if(m_shaderVariables.begin(), m_shaderVariables.end(), [&](const ShaderVariable* var)
		{
			return var->info->Name == name;
		});

	if (find_it == m_shaderVariables.end())
		return E_FAIL;

	(*find_it)->SetRawValue(&value, sizeof(float));

	return S_OK;
}

HRESULT Material::SetVector(const string& name, const V4& value)
{
	if (!m_shader)
		return E_FAIL;

	auto find_it = std::find_if(m_shaderVariables.begin(), m_shaderVariables.end(), [&](const ShaderVariable* var)
		{
			return var->info->Name == name;
		});

	(*find_it)->SetRawValue(&value, sizeof(V4));

	return S_OK;
}

HRESULT Material::SetColor(const string& name, const Color& value)
{
	if (!m_shader)
		return E_FAIL;

	auto find_it = std::find_if(m_shaderVariables.begin(), m_shaderVariables.end(), [&](const ShaderVariable* var)
		{
			return var->info->Name == name;
		});

	if (find_it == m_shaderVariables.end())
		return E_FAIL;

	(*find_it)->SetRawValue(&value, sizeof(Color));

	return S_OK;
}

HRESULT Material::SetMatrix(const string& name, const M4& value)
{
	if (!m_shader)
		return E_FAIL;

	auto find_it = std::find_if(m_shaderVariables.begin(), m_shaderVariables.end(), [&](const ShaderVariable* var)
		{
			return var->info->Name == name;
		});

	if (find_it == m_shaderVariables.end())
		return E_FAIL;

	(*find_it)->SetRawValue(&value, sizeof(M4));

	return S_OK;
}

HRESULT Material::SetTexture(const string& name, ResourceRef<Texture> texture)
{
	if (!m_shader)
		return E_FAIL;

	auto find_it = std::find_if(m_shaderVariables.begin(), m_shaderVariables.end(), [&](const ShaderVariable* var)
		{
			return var->info->Name == name;
		});

	if (find_it == m_shaderVariables.end())
		return E_FAIL;

	(*find_it)->SetTexture(texture);

	return S_OK;
}

HRESULT Material::SetTextures(const string& name, ResourceRef<Texture>* textures, uint count)
{
	if (!m_shader)
		return E_FAIL;

	auto find_it = std::find_if(m_shaderVariables.begin(), m_shaderVariables.end(), [&](const ShaderVariable* var)
		{
			return var->info->Name == name;
		});

	if (find_it == m_shaderVariables.end())
		return E_FAIL;

	(*find_it)->SetTextures(textures, count);

	return S_OK;
}

HRESULT Material::GetRawValue(const string& name, void* out_data, size_t* inout_size)
{
	if (!m_shader)
		return E_FAIL;

	auto find_it = std::find_if(m_shaderVariables.begin(), m_shaderVariables.end(), [&](const ShaderVariable* var)
		{
			return var->info->Name == name;
		});

	if (find_it == m_shaderVariables.end())
		return E_FAIL;

	(*find_it)->GetRawValue(out_data, *inout_size);
	*inout_size = (*find_it)->GetRawValueSize();

	return S_OK;
}

HRESULT Material::GetFloat(const string& name, float* out_value)
{
	if (!m_shader)
		return E_FAIL;

	auto find_it = std::find_if(m_shaderVariables.begin(), m_shaderVariables.end(), [&](const ShaderVariable* var)
		{
			return var->info->Name == name;
		});

	if (find_it == m_shaderVariables.end())
		return E_FAIL;

	(*find_it)->GetRawValue(out_value, sizeof(float));

	return S_OK;
}

HRESULT Material::GetVector(const string& name, V4* out_value)
{
	if (!m_shader)
		return E_FAIL;

	auto find_it = std::find_if(m_shaderVariables.begin(), m_shaderVariables.end(), [&](const ShaderVariable* var)
		{
			return var->info->Name == name;
		});

	if (find_it == m_shaderVariables.end())
		return E_FAIL;

	(*find_it)->GetRawValue(out_value, sizeof(V4));

	return S_OK;
}

HRESULT Material::GetColor(const string& name, Color* out_value)
{
	if (!m_shader)
		return E_FAIL;

	auto find_it = std::find_if(m_shaderVariables.begin(), m_shaderVariables.end(), [&](const ShaderVariable* var)
		{
			return var->info->Name == name;
		});

	if (find_it == m_shaderVariables.end())
		return E_FAIL;

	(*find_it)->GetRawValue(out_value, sizeof(Color));

	return S_OK;
}

HRESULT Material::GetMatrix(const string& name, M4* out_value)
{
	if (!m_shader)
		return E_FAIL;

	auto find_it = std::find_if(m_shaderVariables.begin(), m_shaderVariables.end(), [&](const ShaderVariable* var)
		{
			return var->info->Name == name;
		});

	if (find_it == m_shaderVariables.end())
		return E_FAIL;

	(*find_it)->GetRawValue(out_value, sizeof(M4));

	return S_OK;
}

HRESULT Material::GetTexture(const string& name, ResourceRef<Texture>* out_texture)
{
	if (!m_shader)
		return E_FAIL;

	auto find_it = std::find_if(m_shaderVariables.begin(), m_shaderVariables.end(), [&](const ShaderVariable* var)
		{
			return var->info->Name == name;
		});

	if (find_it == m_shaderVariables.end())
		return E_FAIL;

	*out_texture = (*find_it)->GetTexture();

	return S_OK;
}

HRESULT Material::GetTextures(const string& name, ResourceRef<Texture>** out_textures, uint* out_count)
{
	if (!m_shader)
		return E_FAIL;

	auto find_it = std::find_if(m_shaderVariables.begin(), m_shaderVariables.end(), [&](const ShaderVariable* var)
		{
			return var->info->Name == name;
		});

	if (find_it == m_shaderVariables.end())
		return E_FAIL;

	*out_textures = (*find_it)->GetTextures();
	*out_count = (*find_it)->GetElementCount();

	return S_OK;
}

HRESULT Material::GetTechniqueCount(uint& out_techniqueCount) const
{
	if (!m_shader)
		return E_FAIL;

	return m_shader->GetTechniqueCount(out_techniqueCount);
}

HRESULT Material::GetPassCount(uint techniqueIndex, uint& out_passCount) const
{
	if (!m_shader)
		return E_FAIL;

	return m_shader->GetPassCount(techniqueIndex, out_passCount);
}

uint Material::GetPassCountOfAppliedTechnique() const
{
	if (!m_shader)
		return 0;

	uint passCount = 0;
	if (FAILED(m_shader->GetPassCount(m_techniqueIndex, passCount)))
		return 0;

	return passCount;
}

HRESULT Material::GetRenderGroupOfAppliedTechnique(uint passIndex, RenderGroup& out_renderGroup) const
{
	if (!m_shader)
		return E_FAIL;

	CompiledShaderDesc* shaderDesc = m_shader->GetShaderDesc();
	if (!shaderDesc)
		return E_FAIL;

	TechniqueDesc* techniqueDesc = shaderDesc->GetTechniqueDesc(m_techniqueIndex);
	if (!techniqueDesc)
		return E_FAIL;

	PassDesc* passDesc = techniqueDesc->GetPassDesc(passIndex);
	if (!passDesc)
		return E_FAIL;

	out_renderGroup = passDesc->GetRenderGroup();

	return S_OK;
}

HRESULT Material::GetRenderGroupOrderOfAppliedTechnique(uint passIndex, int& out_renderGroupOrder) const
{
	if (!m_shader)
		return E_FAIL;

	CompiledShaderDesc* shaderDesc = m_shader->GetShaderDesc();
	if (!shaderDesc)
		return E_FAIL;

	TechniqueDesc* techniqueDesc = shaderDesc->GetTechniqueDesc(m_techniqueIndex);
	if (!techniqueDesc)
		return E_FAIL;

	PassDesc* passDesc = techniqueDesc->GetPassDesc(passIndex);
	if (!passDesc)
		return E_FAIL;

	out_renderGroupOrder = passDesc->GetRenderGroupOrder();

	return S_OK;
}

HRESULT Material::GetCullingFlagOfAppliedTechnique(uint passIndex, bool& out_cullingFlag) const
{
	if (!m_shader)
		return E_FAIL;

	CompiledShaderDesc* shaderDesc = m_shader->GetShaderDesc();
	if (!shaderDesc)
		return E_FAIL;

	TechniqueDesc* techniqueDesc = shaderDesc->GetTechniqueDesc(m_techniqueIndex);
	if (!techniqueDesc)
		return E_FAIL;

	PassDesc* passDesc = techniqueDesc->GetPassDesc(passIndex);
	if (!passDesc)
		return E_FAIL;

	out_cullingFlag = passDesc->IsCulling();

	return S_OK;
}

HRESULT Material::GetInstancingFlagOfAppliedTechnique(uint passIndex, bool& out_instancingFlag) const
{
	if (!m_shader)
		return E_FAIL;

	CompiledShaderDesc* shaderDesc = m_shader->GetShaderDesc();
	if (!shaderDesc)
		return E_FAIL;

	TechniqueDesc* techniqueDesc = shaderDesc->GetTechniqueDesc(m_techniqueIndex);
	if (!techniqueDesc)
		return E_FAIL;

	PassDesc* passDesc = techniqueDesc->GetPassDesc(passIndex);
	if (!passDesc)
		return E_FAIL;

	out_instancingFlag = passDesc->IsInstancing();

	return S_OK;
}

HRESULT Material::GetDrawShadowFlagOfAppliedTechnique(uint passIndex, bool& out_drawShadowFlag) const
{
	if (!m_shader)
		return E_FAIL;

	CompiledShaderDesc* shaderDesc = m_shader->GetShaderDesc();
	if (!shaderDesc)
		return E_FAIL;

	TechniqueDesc* techniqueDesc = shaderDesc->GetTechniqueDesc(m_techniqueIndex);
	if (!techniqueDesc)
		return E_FAIL;

	PassDesc* passDesc = techniqueDesc->GetPassDesc(passIndex);
	if (!passDesc)
		return E_FAIL;

	out_drawShadowFlag = passDesc->IsDrawShadow();

	return S_OK;
}

HRESULT Material::GetShadowPassFlagOfAppliedTechnique(uint passIndex, bool& out_shadowPassFlag) const
{
	if (!m_shader)
		return E_FAIL;

	CompiledShaderDesc* shaderDesc = m_shader->GetShaderDesc();
	if (!shaderDesc)
		return E_FAIL;

	TechniqueDesc* techniqueDesc = shaderDesc->GetTechniqueDesc(m_techniqueIndex);
	if (!techniqueDesc)
		return E_FAIL;

	PassDesc* passDesc = techniqueDesc->GetPassDesc(passIndex);
	if (!passDesc)
		return E_FAIL;

	out_shadowPassFlag = passDesc->IsShadowPass();

	return S_OK;
}

bool Material::IsValid() const
{
	if (!m_shader)
		return false;

	uint techniqueCount = 0;
	
	if (FAILED(GetTechniqueCount(techniqueCount)))
		return false;

	if (m_techniqueIndex >= techniqueCount)
		return false;

	uint passCount = 0;

	if (FAILED(GetPassCount(m_techniqueIndex, passCount)))
		return false;

	if (passCount == 0)
		return false;

	return true;
}

ResourceRef<Shader> Material::GetShader() const
{
	return m_shader;
}

ResourceRef<Material> Material::CreateMaterialByShaderM(ResourceManagement* management, ResourceRef<Shader> shader, const tstring& resourceKey)
{
	if (!management)
		return nullptr;

	ResourceRef<ResourceObject> find = management->Find(resourceKey);
	if (find)
		return find;

	if (!shader)
		return nullptr;

	return new Material(management, true, resourceKey, shader);
}

ResourceRef<Material> Material::CreateMaterialByShaderUM(ResourceManagement* management, ResourceRef<Shader> shader)
{
	if (!management)
		return nullptr;

	if (!shader)
		return nullptr;

	return new Material(management, false, TEXT(""), shader);
}

ResourceRef<Material> Material::CopyMaterialM(ResourceManagement* management, ResourceRef<Material> material, const tstring& resourceKey)
{
	if (!management)
		return nullptr;

	if (!material)
		return nullptr;

	if (management->Exist(resourceKey))
		return nullptr;

	return new Material(management, true, resourceKey, *material);
}

ResourceRef<Material> Material::CopyMaterialUM(ResourceManagement* management, ResourceRef<Material> material)
{
	if (!management)
		return nullptr;

	if (!material)
		return nullptr;

	return new Material(management, false, TEXT(""), *material);
}

ResourceRef<Material> Material::LoadMaterialM(ResourceManagement* management, const tstring& jsonPath)
{
	return LoadMaterialFromJsonCommon(management, jsonPath, &jsonPath);
}

ResourceRef<Material> Material::LoadMaterialUM(ResourceManagement* management, const tstring& jsonPath)
{
	return LoadMaterialFromJsonCommon(management, jsonPath, nullptr);
}

string Material::ToJson() const
{
	Json::Value root;
	root["shader"] = tstring_to_string(m_shader->path);

	Json::Value variables;

	for (uint i = 0; i < (uint)m_shaderVariables.size(); ++i)
	{
		auto& shaderVariable = m_shaderVariables[i];

		Json::Value variable;
		variable["name"] = shaderVariable->info->Name;
		variable["semantic"] = shaderVariable->info->Semantic;
		variable["arrayCount"] = shaderVariable->info->ArrayCount;
		variable["array"] = shaderVariable->info->IsArray();
		variable["type"] = (uint)shaderVariable->info->Type;
		
		Json::Value data;
		for (uint j = 0; j < shaderVariable->info->ArrayCount; ++j)
		{
			switch (shaderVariable->info->Type)
			{
				case ShaderVariableType::Bool: 
					data[j] = ((const bool*)(shaderVariable->constRawValuePointer))[j]; break;
				case ShaderVariableType::Int: 
					data[j] = ((const int*)(shaderVariable->constRawValuePointer))[j]; break;
				case ShaderVariableType::UInt: 
					data[j] = ((const uint*)(shaderVariable->constRawValuePointer))[j]; break;
				case ShaderVariableType::Float: 
					data[j] = ((const float*)(shaderVariable->constRawValuePointer))[j]; break;
				case ShaderVariableType::Double: 
					data[j] = ((const double*)(shaderVariable->constRawValuePointer))[j]; break;
				case ShaderVariableType::Vector: 
					data[j] = JsonUtility::Parse(((const V4*)(shaderVariable->constRawValuePointer))[j]); break;
				case ShaderVariableType::Matrix: 
					data[j] = JsonUtility::Parse(((const M4*)(shaderVariable->constRawValuePointer))[j]); break;
				case ShaderVariableType::Texture1D:
				case ShaderVariableType::Texture2D:
				case ShaderVariableType::Texture3D:
				case ShaderVariableType::Texture1DArray:
				case ShaderVariableType::Texture2DArray:
					data[j] = tstring_to_string(shaderVariable->textures[j]->path);
					break;
			}
		}

		variable["data"] = data;

		variables[i] = variable;
	}
	
	root["variables"] = variables;

	Json::StreamWriterBuilder builder;
	return Json::writeString(builder, root);
}

ResourceRef<Material> Material::LoadMaterialFromJsonCommon(ResourceManagement* management, const tstring& jsonPath, const tstring* nullable_resourceKey)
{
	if (!management)
		return nullptr;

	if (nullable_resourceKey && management->Exist(*nullable_resourceKey))
		return nullptr;

	string error;
	std::ifstream ifs;
	ifs.open(jsonPath, ios_base::in);
	if (!ifs.is_open())
		return nullptr;
	std::stringstream buffer;
	buffer << ifs.rdbuf();
	string json = buffer.str();
	ifs.close();
	buffer.clear();

	Json::Value root;
	Json::CharReaderBuilder builder;
	unique_ptr<Json::CharReader> reader(builder.newCharReader());
	if (!reader->parse(json.c_str(), json.c_str() + json.length(), &root, &error))
		return nullptr;

	const string& shaderPath = root["shader"].asString();
	ResourceRef<Shader> shader = management->Find(string_to_tstring(shaderPath));
	if (!shader)
		return nullptr;

	bool isManagement = nullable_resourceKey != nullptr;
	tstring resourceKey = nullable_resourceKey ? *nullable_resourceKey : jsonPath;
	Material* material = new Material(management, isManagement, resourceKey, shader);
	const Json::Value& variables = root["variables"];

	for (uint i = 0; i < variables.size(); ++i)
	{
		const Json::Value& variable = variables[i];

		for (auto& shaderVariable : material->m_shaderVariables)
		{
			if (shaderVariable->info->Name == variable["name"].asString() &&
				shaderVariable->info->Semantic == variable["semantic"].asString() &&
				shaderVariable->info->ArrayCount == variable["arrayCount"].asUInt() &&
				shaderVariable->info->IsArray() == variable["array"].asBool() &&
				shaderVariable->info->Type == (ShaderVariableType)variable["type"].asUInt())
			{
				V4 vector;
				M4 matrix;
				ResourceRef<Texture> texture;

				for (uint j = 0; j < variable["data"].size(); ++j)
				{
					switch (shaderVariable->info->Type)
					{
						case ShaderVariableType::Bool:
							shaderVariable->SetBool(variable["data"][j].asBool(), j); break;
						case ShaderVariableType::Int:
							shaderVariable->SetInt(variable["data"][j].asInt(), j); break;
						case ShaderVariableType::UInt:
							shaderVariable->SetUInt(variable["data"][j].asUInt(), j); break;
						case ShaderVariableType::Float:
							shaderVariable->SetFloat(variable["data"][j].asFloat(), j); break;
						case ShaderVariableType::Double:
							shaderVariable->SetDouble(variable["data"][j].asDouble(), j); break;
						case ShaderVariableType::Vector:
							if (!JsonUtility::ToVector4(variable["data"][j].asString(), vector)) break;
							shaderVariable->SetVector(vector, j); break;
						case ShaderVariableType::Matrix:
							if (!JsonUtility::ToMatrix4x4(variable["data"][j].asString(), matrix)) break;
							shaderVariable->SetMatrix(matrix, j); break;
						case ShaderVariableType::Texture1D:
						case ShaderVariableType::Texture2D:
						case ShaderVariableType::Texture3D:
						case ShaderVariableType::Texture1DArray:
						case ShaderVariableType::Texture2DArray:
							string texturePath = variable["data"][j].asString();
							if (texturePath == "white")
								texture = management->builtInResources->whiteTexture;
							else if (texturePath == "black")
								texture = management->builtInResources->blackTexture;
							else if (texturePath == "red")
								texture = management->builtInResources->redTexture;
							else if (texturePath == "green")
								texture = management->builtInResources->greenTexture;
							else if (texturePath == "blue")
								texture = management->builtInResources->blueTexture;
							else if (texturePath == "clear")
								texture = management->builtInResources->clearTexture;
							else if (texturePath == "normal")
								texture = management->builtInResources->normalTexture;
							else
								texture = management->Find(string_to_tstring(texturePath));
							shaderVariable->SetTextureByIndex(texture, j); break;
					}
				}
				break;
			}
		}
	}

	return material;
}

HRESULT Material::GetEffectDesc(Com<ID3DX11Effect>& out_effect) const
{
	if (!m_shader)
		return E_FAIL;

	auto shaderDesc = m_shader->GetShaderDesc();
	if (!shaderDesc)
		return E_FAIL;

	out_effect = shaderDesc->GetEffect();

	return S_OK;
}

HRESULT Material::SetInputLayout(Com<ID3D11DeviceContext> deviceContext, uint techniqueIndex, uint passIndex)
{
	if (!m_shader)
		return E_FAIL;

	return m_shader->SetInputLayout(deviceContext, techniqueIndex, passIndex);
}

HRESULT Material::ApplyPass(Com<ID3D11DeviceContext> deviceContext, uint techniqueIndex, uint passIndex)
{
	if (!m_shader)
		return E_FAIL;

	return m_shader->ApplyPass(deviceContext, techniqueIndex, passIndex);
}

void Material::ApplyVariables()
{
	for (auto& var : m_shaderVariables)
		var->Apply();
}

void Material::ApplySpecificVariables(Com<ID3D11DeviceContext> deviceContext, ICamera* camera)
{
	for (auto& var : m_specificShaderVariables)
		var->Apply(deviceContext, camera);
}

void Material::SetupShaderVariables()
{
	for (uint i = 0; i < m_shader->variableCount; ++i)
	{
		const ShaderVariableInfo* variableInfo = m_shader->FindVariableByIndex(i);

		if (variableInfo->Semantic == "ENGINE")
			continue;
		if (variableInfo->Type == ShaderVariableType::Unknown)
			continue;
		if (variableInfo->Type == ShaderVariableType::Other)
			continue;

		if (variableInfo->Semantic == "")
		{
			ShaderVariable* variable = new ShaderVariable(this, variableInfo);

			//// Default: White Texture
			//if (variable->isTexture)
			//{
			//	if (!variable->isArray)
			//	{
			//		variable->SetTexture(system->resourceManagement->builtInResources->whiteTexture);
			//	}
			//	else
			//	{
			//		ResourceRef<Texture>* arr = new ResourceRef<Texture>[variable->elementCount]{};
			//		for (uint i = 0; i < 32 && i < variable->elementCount; ++i)
			//			arr[i] = system->resourceManagement->builtInResources->whiteTexture;
			//		variable->SetTextures(arr, variable->elementCount);
			//		SafeDeleteArray(arr);
			//	}
			//}

			ApplyAnnotation(variable);

			m_shaderVariables.emplace_back(variable);
		}
		else if (variableInfo->Semantic == "TIME")
			m_specificShaderVariables.emplace_back(new SSVTime(this, variableInfo));
		else if (variableInfo->Semantic == "UNSCALED_TIME")
			m_specificShaderVariables.emplace_back(new SSVUnscaledTime(this, variableInfo));
		else if (variableInfo->Semantic == "DELTA_TIME")
			m_specificShaderVariables.emplace_back(new SSVDeltaTime(this, variableInfo));
		else if (variableInfo->Semantic == "FIXED_DELTA_TIME")
			m_specificShaderVariables.emplace_back(new SSVFixedDeltaTime(this, variableInfo));
		else if (variableInfo->Semantic == "GRAB_TEXTURE")
			m_specificShaderVariables.emplace_back(new SSVGrabTexture(this, variableInfo));
		else if (variableInfo->Semantic == "DEPTH_TEXTURE")
			m_specificShaderVariables.emplace_back(new SSVDepthTexture(this, variableInfo));
	}
}

void Material::ApplyAnnotation(ShaderVariable* variable)
{
	for (uint j = 0; j < variable->info->annotationCount; ++j)
	{
		const ShaderVariableInfo::Annotation& annotation = variable->info->GetAnnotation(j);
		if (annotation.name == "default")
		{
			bool isTexture2D = false;
			bool isScalar = false;
			bool isVector = false;
			switch (variable->info->Type)
			{
				case ShaderVariableType::Texture2D:
					isTexture2D = true;
					break;
				case ShaderVariableType::Bool:
				case ShaderVariableType::Int:
				case ShaderVariableType::UInt:
				case ShaderVariableType::Float:
				case ShaderVariableType::Double:
					isScalar = true;
					break;
				case ShaderVariableType::Vector:
					isVector = true;
					break;
			}

			switch (annotation.type)
			{
				case ShaderVariableInfo::Annotation::Type::Scalar:
					if (isScalar)
					{
						variable->SetRawValue(&annotation.scalar, variable->info->Size);
					}
					break;
				case ShaderVariableInfo::Annotation::Type::Text:
					if (isTexture2D)
					{
						if (annotation.text == "white")
							variable->SetTexture(system->resource->builtInResources->whiteTexture);
						else if (annotation.text == "black")
							variable->SetTexture(system->resource->builtInResources->blackTexture);
						else if (annotation.text == "red")
							variable->SetTexture(system->resource->builtInResources->redTexture);
						else if (annotation.text == "green")
							variable->SetTexture(system->resource->builtInResources->greenTexture);
						else if (annotation.text == "blue")
							variable->SetTexture(system->resource->builtInResources->blueTexture);
						else if (annotation.text == "clear")
							variable->SetTexture(system->resource->builtInResources->clearTexture);
						else if (annotation.text == "normal")
							variable->SetTexture(system->resource->builtInResources->normalTexture);
					}
					break;
				case ShaderVariableInfo::Annotation::Type::Vector:
					if (isVector)
					{
						variable->SetRawValue(&annotation.vector, sizeof(V4));
					}
					break;
			}
		}
	}
}
