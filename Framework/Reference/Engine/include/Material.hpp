#pragma once
#include "Material.h"
#include "ResourceManagement.h"
#include "tstring.h"

template<class MaterialType>
inline ResourceRef<MaterialType> Material::CreateManagedMaterial(ResourceManagement* management, const tstring& resourceKey)
{
	if (!management)
		return ResourceRef<MaterialType>();

	ResourceRef<MaterialType> find = management->Find(resourceKey);
	if (find)
		return find;

	Material* material = new MaterialType();

	if (!material->Initialize(management, true, resourceKey, TEXT("")))
	{
		tstring materialTypeName = string_to_tstring(typeid(*material).name());
		tstring errorMessage = TEXT("Failed to initialize on material(") + materialTypeName + TEXT(") creating: ") + material->GetShaderPath();
		ERROR_MESSAGE_NT(errorMessage.c_str());

		SafeDelete(material);
		return ResourceRef<MaterialType>();
	}

	if (!material->SetupShader())
	{
		tstring materialTypeName = string_to_tstring(typeid(*material).name());
		tstring errorMessage = TEXT("Failed to load shader on material(") + materialTypeName + TEXT(") creating: ") + material->GetShaderPath();
		ERROR_MESSAGE_NT(errorMessage.c_str());

		SafeDelete(material);
		return ResourceRef<MaterialType>();
	}

	if (!material->SetupDefaultDiffuseTexture())
	{
		tstring materialTypeName = string_to_tstring(typeid(*material).name());
		tstring errorMessage = TEXT("Failed to load default diffuse texture on material(") + materialTypeName + TEXT(")");
		ERROR_MESSAGE_NT(errorMessage.c_str());

		SafeDelete(material);
		return ResourceRef<MaterialType>();
	}

	material->OnCreated();

	IResourceManagement* iManagement = management;
	iManagement->AddManagedResource(material);
	return ResourceRef<MaterialType>(material);
}

template<class MaterialType>
inline ResourceRef<MaterialType> Material::CreateManagedMaterial(ResourceManagement* management, const tstring& resourceKey, const tstring& groupName)
{
	if (!management)
		return ResourceRef<MaterialType>();

	ResourceRef<MaterialType> find = management->Find(resourceKey);
	if (find)
		return find;

	Material* material = new MaterialType(management, resourceKey, groupName);

	if (!material->Initialize(management, true, resourceKey, groupName))
	{
		tstring materialTypeName = string_to_tstring(typeid(*material).name());
		tstring errorMessage = TEXT("Failed to initialize on material(") + materialTypeName + TEXT(") creating: ") + material->GetShaderPath();
		ERROR_MESSAGE_NT(errorMessage.c_str());

		SafeDelete(material);
		return ResourceRef<MaterialType>();
	}

	if (!material->SetupShader())
	{
		tstring materialTypeName = string_to_tstring(typeid(*material).name());
		tstring errorMessage = TEXT("Failed to load shader on material(") + materialTypeName + TEXT(") creating: ") + material->GetShaderPath();
		ERROR_MESSAGE_NT(errorMessage.c_str());

		SafeDelete(material);
		return ResourceRef<MaterialType>();
	}

	if (!material->SetupDefaultDiffuseTexture())
	{
		tstring materialTypeName = string_to_tstring(typeid(*material).name());
		tstring errorMessage = TEXT("Failed to load default diffuse texture on material(") + materialTypeName + TEXT(")");
		ERROR_MESSAGE_NT(errorMessage.c_str());

		SafeDelete(material);
		return ResourceRef<MaterialType>();
	}

	material->OnCreated();

	IResourceManagement* iManagement = management;
	iManagement->AddManagedResource(material);
	return ResourceRef<MaterialType>(material);
}

template<class MaterialType>
inline ResourceRef<MaterialType> Material::CreateUnmanagedMaterial(ResourceManagement* management)
{
	if (!management)
		return ResourceRef<MaterialType>();

	Material* material = new MaterialType();

	if (!material->Initialize(management, false, TEXT(""), TEXT("")))
	{
		tstring materialTypeName = string_to_tstring(typeid(*material).name());
		tstring errorMessage = TEXT("Failed to initialize on material(") + materialTypeName + TEXT(") creating: ") + material->GetShaderPath();
		ERROR_MESSAGE_NT(errorMessage.c_str());

		SafeDelete(material);
		return ResourceRef<MaterialType>();
	}

	if (!material->SetupShader())
	{
		tstring materialTypeName = string_to_tstring(typeid(*material).name());
		tstring errorMessage = TEXT("Failed to load shader on material(") + materialTypeName + TEXT(") creating: ") + material->GetShaderPath();
		ERROR_MESSAGE_NT(errorMessage.c_str());

		SafeDelete(material);
		return ResourceRef<MaterialType>();
	}

	if (!material->SetupDefaultDiffuseTexture())
	{
		tstring materialTypeName = string_to_tstring(typeid(*material).name());
		tstring errorMessage = TEXT("Failed to load default diffuse texture on material(") + materialTypeName + TEXT(")");
		ERROR_MESSAGE_NT(errorMessage.c_str());

		SafeDelete(material);
		return ResourceRef<MaterialType>();
	}

	material->OnCreated();

	return ResourceRef<MaterialType>(material);
}