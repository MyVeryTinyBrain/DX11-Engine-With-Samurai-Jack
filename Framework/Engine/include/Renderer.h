#pragma once

#include "Component.h"
#include "ResourceRef.h"
#include "IRendererBoundsOp.h"

ENGINE_BEGIN
class Material;
class Mesh;
class ENGINE_API Renderer abstract : public Component, public IRendererBoundsOp
{
public:

	virtual void Render() override = 0;

	virtual Bounds GetBounds() const = 0;

public:

	ResourceRef<Material> GetMaterial() const;

	void SetMaterial(ResourceRef<Material> material);

	void SetMaterialByIndex(uint index, ResourceRef<Material> material);

	const vector<ResourceRef<Material>>& GetMaterials() const;

	void SetMaterials(const vector<ResourceRef<Material>>& materials);

	ResourceRef<Material> GetMaterialByIndex(uint index) const;

	void SetMaterialCount(uint count);

	uint GetMaterialCount() const;

	inline uint GetLayerIndex() const { return m_layerIndex; }

	inline void SetlayerIndex(uint8_t layerIndex) { m_layerIndex = layerIndex; };

	virtual bool IsValid() const;

	ResourceRef<Mesh> GetProtectedMesh() const;

	_declspec(property(get = GetMaterial, put = SetMaterial)) ResourceRef<Material> material;
	_declspec(property(get = GetMaterials, put = SetMaterials)) const vector<ResourceRef<Material>>& materials;
	_declspec(property(get = GetMaterialCount)) uint materialCount;
	_declspec(property(get = IsValid)) bool isValid;

protected:

	ResourceRef<Mesh> m_mesh;

	vector<ResourceRef<Material>> m_materials;

	uint8_t m_layerIndex = 0;

};

ENGINE_END
