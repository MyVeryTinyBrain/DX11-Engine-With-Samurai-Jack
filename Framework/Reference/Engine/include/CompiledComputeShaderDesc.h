#pragma once

ENGINE_BEGIN

class ENGINE_API CompiledComputeShaderDesc
{
protected:

	CompiledComputeShaderDesc(ID3D11ComputeShader* computeShader);

public:

	~CompiledComputeShaderDesc();

public:

	Com<ID3D11ComputeShader> GetComputeShader() const;

public:

	static CompiledComputeShaderDesc* CreateCompiledShaderFromFile(Com<ID3D11Device> device, const tstring& path);

private:

	ID3D11ComputeShader* m_computeShader = nullptr;
};

ENGINE_END
