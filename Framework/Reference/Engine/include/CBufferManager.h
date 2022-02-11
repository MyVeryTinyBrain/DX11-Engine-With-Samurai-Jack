#pragma once

#include "CBufferTypes.h"

ENGINE_BEGIN
class GraphicSystem;
class ENGINE_API CBufferManager
{
public:

	CBufferManager(GraphicSystem* graphicSystem);

	~CBufferManager();

	HRESULT Initialize();

public:

	HRESULT BeginApply(Com<ID3DX11Effect> effect);

	void EndApply();

	void ApplyWorldMatrixBuffer(const M4& world);

	void ApplyCameraBuffer(const V3& position, const V3& direction, const M4& view, const M4& proj, float Near, float Far);

	void ApplyBoneMatricesUsageBuffer(bool use);

	BoneMatricesCBuffer* GetBoneMatricesBufferData();

	void ApplyBoneMatrices();

private:

	HRESULT CreateBuffer(size_t size, ID3D11Buffer** out_buffer);

	HRESULT MapBuffer(ID3D11Buffer* buffer, D3D11_MAPPED_SUBRESOURCE* out_mappedSubresource);

	HRESULT UnmapBuffer(ID3D11Buffer* buffer);

	HRESULT SetCBuffer(ID3D11Buffer* buffer, Com<ID3DX11Effect> effect, const char* CBufferName);

private:

	ID3D11Buffer* m_worldMatrixBuffer = nullptr;

	ID3D11Buffer* m_cameraBuffer = nullptr;

	ID3D11Buffer* m_boneMatricesUsageBuffer = nullptr;

	BoneMatricesCBuffer m_boneMatricesBufferData = {};

	ID3D11Buffer* m_boneMatricesBuffer = nullptr;

	Com<ID3DX11Effect> m_settedEffect;

	GraphicSystem* m_graphicSystem = nullptr;
};

ENGINE_END
