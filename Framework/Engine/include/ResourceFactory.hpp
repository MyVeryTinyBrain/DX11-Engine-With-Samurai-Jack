#pragma once
#include "ResourceFactory.h"
#include "ResourceRef.h"
#include "Material.h"

template<class MaterialType>
inline ResourceRef<MaterialType> ResourceFactory::CreateManagedMaterial(const tstring& resourceKey)
{
	return Material::CreateManagedMaterial<MaterialType>(m_management, resourceKey);
}

template<class MaterialType>
inline ResourceRef<MaterialType> ResourceFactory::CreateManagedMaterial(const tstring& resourceKey, const tstring& groupName)
{
	return Material::CreateManagedMaterial<MaterialType>(m_management, resourceKey, groupName);
}

template<class MaterialType>
inline ResourceRef<MaterialType> ResourceFactory::CreateUnmanagedMaterial()
{
	return Material::CreateUnmanagedMaterial<MaterialType>(m_management);
}