#pragma once

ENGINE_BEGIN
class PassDesc;
class ENGINE_API IMaterial abstract
{
public:

	virtual ~IMaterial() = default;

	virtual void SetMaterialValues() = 0;

	virtual HRESULT GetTechniqueCount(size_t& out_techniqueCount) const = 0;

	virtual HRESULT GetPassCount(size_t techniqueIndex, size_t& out_passCount) const = 0;

	virtual HRESULT GetEffectDesc(Com<ID3DX11Effect>& out_effect) const = 0;

	virtual HRESULT SetInputLayout(Com<ID3D11DeviceContext> deviceContext, size_t techniqueIndex, size_t passIndex) = 0;

	virtual HRESULT ApplyPass(Com<ID3D11DeviceContext> deviceContext, size_t techniqueIndex, size_t passIndex) = 0;
};

ENGINE_END
