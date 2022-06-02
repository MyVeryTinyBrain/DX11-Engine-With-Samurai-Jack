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

	// 0번째 재질을 설정하거나 가져옵니다.

	ResourceRef<Material> GetMaterial() const;
	void SetMaterial(ResourceRef<Material> material);

	// 지정한 인덱스의 재질을 설정한거나 가져옵니다.

	void SetMaterialByIndex(uint index, ResourceRef<Material> material);
	const vector<ResourceRef<Material>>& GetMaterials() const;

	void SetMaterials(const vector<ResourceRef<Material>>& materials);
	ResourceRef<Material> GetMaterialByIndex(uint index) const;

	// 사용할수 있는 재질의 수를 설정하거나 가져옵니다.

	void SetMaterialCount(uint count);
	uint GetMaterialCount() const;

	inline uint GetLayerIndex() const { return m_layerIndex; }
	inline void SetlayerIndex(uint8_t layerIndex) { m_layerIndex = layerIndex; };

	// 재질을 비웁니다.

	void ClearMaterials();

	// 적용된 메쉬에 알맞은 재질의 수를 설정하고
	// 알맞은 StandardMaterial을 관리되지 않는 리소스로 생성해 적용합니다.
	// 메쉬가 설정되어 있지 않다면 재질을 비우게 됩니다.

	void SetupStandardMaterials();

	// 메쉬에 기본 재질을 설정합니다.

	void SetupDefaultMaterials();

	// 인스턴싱에 사용되는 데이터를 설정하거나 가져옵니다.
	// 최대 0, 1, 2, 3번의 인덱스를 사용할수 있습니다.

	void SetInstanceData(uint index, const Real4& data);
	const Real4& GetInstanceData(uint index);
	const Real4* GetInstanceDataArray() const { return m_instanceDatas; }

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

	Real4 m_instanceDatas[4] = {};
};

ENGINE_END
