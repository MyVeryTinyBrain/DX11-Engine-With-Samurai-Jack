#pragma once

#include "ShaderTypes.h"

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

	virtual ~ISpecificShaderVariable() = default;

	virtual void Apply(ICamera* camera) = 0;

protected:

	const System*				m_system;
	Material*					m_material;
	Shader*						m_shader;
	const ShaderVariableInfo* 	m_variableInfo;
};

ENGINE_END
