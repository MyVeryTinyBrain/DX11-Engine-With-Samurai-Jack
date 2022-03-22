#pragma once

ENGINE_BEGIN
class PassDesc;
class ENGINE_API IMaterial abstract
{
public:

	virtual ~IMaterial() = default;

	virtual void SetMaterialValues() = 0;

	virtual HRESULT GetTechniqueCount(uint& out_techniqueCount) const = 0;

	virtual HRESULT GetPassCount(uint techniqueIndex, uint& out_passCount) const = 0;

	virtual HRESULT GetEffectDesc(Com<ID3DX11Effect>& out_effect) const = 0;

	virtual HRESULT SetInputLayout(Com<ID3D11DeviceContext> deviceContext, uint techniqueIndex, uint passIndex) = 0;

	virtual HRESULT ApplyPass(Com<ID3D11DeviceContext> deviceContext, uint techniqueIndex, uint passIndex) = 0;
};

ENGINE_END
