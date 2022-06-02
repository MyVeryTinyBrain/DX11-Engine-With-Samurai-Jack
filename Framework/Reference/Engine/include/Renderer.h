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

	// 0��° ������ �����ϰų� �����ɴϴ�.

	ResourceRef<Material> GetMaterial() const;
	void SetMaterial(ResourceRef<Material> material);

	// ������ �ε����� ������ �����Ѱų� �����ɴϴ�.

	void SetMaterialByIndex(uint index, ResourceRef<Material> material);
	const vector<ResourceRef<Material>>& GetMaterials() const;

	void SetMaterials(const vector<ResourceRef<Material>>& materials);
	ResourceRef<Material> GetMaterialByIndex(uint index) const;

	// ����Ҽ� �ִ� ������ ���� �����ϰų� �����ɴϴ�.

	void SetMaterialCount(uint count);
	uint GetMaterialCount() const;

	inline uint GetLayerIndex() const { return m_layerIndex; }
	inline void SetlayerIndex(uint8_t layerIndex) { m_layerIndex = layerIndex; };

	// ������ ���ϴ�.

	void ClearMaterials();

	// ����� �޽��� �˸��� ������ ���� �����ϰ�
	// �˸��� StandardMaterial�� �������� �ʴ� ���ҽ��� ������ �����մϴ�.
	// �޽��� �����Ǿ� ���� �ʴٸ� ������ ���� �˴ϴ�.

	void SetupStandardMaterials();

	// �޽��� �⺻ ������ �����մϴ�.

	void SetupDefaultMaterials();

	// �ν��Ͻ̿� ���Ǵ� �����͸� �����ϰų� �����ɴϴ�.
	// �ִ� 0, 1, 2, 3���� �ε����� ����Ҽ� �ֽ��ϴ�.

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
