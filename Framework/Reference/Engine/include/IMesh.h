#pragma once

ENGINE_BEGIN

class ENGINE_API IMesh abstract
{
public:

	virtual ~IMesh() = default;

	virtual HRESULT ApplyVertexBuffer(Com<ID3D11DeviceContext> deviceContext) = 0;

	virtual HRESULT ApplyIndexBuffer(Com<ID3D11DeviceContext> deviceContext) = 0;

	virtual HRESULT DrawSubMesh(Com<ID3D11DeviceContext> deviceContext, uint subMeshIndex) = 0;
	virtual HRESULT DrawSubMesh(Com<ID3D11DeviceContext> deviceContext, uint subMeshIndex, uint primitiveCount) = 0;

	virtual HRESULT ApplyVertexAndInstanceBuffer(Com<ID3D11DeviceContext> deviceContext, Com<ID3D11Buffer> instanceBuffer) = 0;

	virtual HRESULT DrawInstanceSubMesh(Com<ID3D11DeviceContext> deviceContext, uint subMeshIndex, uint instanceCount) = 0;
	virtual HRESULT DrawInstanceSubMesh(Com<ID3D11DeviceContext> deviceContext, uint subMeshIndex, uint instanceCount, uint primitiveCount) = 0;
};

ENGINE_END
