#pragma once

#include "ShaderType.h"

ENGINE_BEGIN
class System;
class Material;
class Shader;
class ICamera;
class ShaderVariableInfo;
class ENGINE_API ISpecificShaderVariable abstract
{
public:

	ISpecificShaderVariable(Material* material, const ShaderVariableInfo* shaderVariableInfo);
	ISpecificShaderVariable(Material* material, const ISpecificShaderVariable& other);

	virtual ~ISpecificShaderVariable() = default;

	virtual void Apply(Com<ID3D11DeviceContext> deviceContext, ICamera* camera) = 0;

	virtual ISpecificShaderVariable* Copy(Material* material, const ISpecificShaderVariable& other) const = 0;

protected:

	const System*				m_system;
	Material*					m_material;
	Shader*						m_shader;
	const ShaderVariableInfo* 	m_variableInfo;
};

ENGINE_END
