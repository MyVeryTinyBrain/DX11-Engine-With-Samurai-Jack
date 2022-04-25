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

	void ApplyWorldMatrixBuffer(Com<ID3D11DeviceContext> deviceContext, const M4& world);

	void ApplyCameraBuffer(Com<ID3D11DeviceContext> deviceContext, const V3& position, const V3& direction, const M4& view, const M4& proj, uint2 size, float Near, float Far);

	void ApplyBoneMatricesUsageBuffer(Com<ID3D11DeviceContext> deviceContext, bool use);

	BoneMatricesCBuffer* GetBoneMatricesBufferData();

	void ApplyBoneMatrices(Com<ID3D11DeviceContext> deviceContext);

private:

	HRESULT CreateBuffer(size_t size, ID3D11Buffer** out_buffer);

	HRESULT MapBuffer(Com<ID3D11DeviceContext> deviceContext, ID3D11Buffer* buffer, D3D11_MAPPED_SUBRESOURCE* out_mappedSubresource);

	HRESULT UnmapBuffer(Com<ID3D11DeviceContext> deviceContext, ID3D11Buffer* buffer);

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
