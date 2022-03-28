#include "EnginePCH.h"
#include "ISpecificShaderVariable.h"
#include "Material.h"
#include "Shader.h"

ISpecificShaderVariable::ISpecificShaderVariable(Material* material, const ShaderVariableInfo* shaderVariableInfo)
{
	m_system = material->system;
	m_material = material;
	m_shader = material->GetShader();
	m_variableInfo = shaderVariableInfo;
}

ISpecificShaderVariable::ISpecificShaderVariable(Material* material, const ISpecificShaderVariable& other)
{
	m_system = other.m_system;
	m_material = material;
	m_shader = material->GetShader();
	m_variableInfo = other.m_variableInfo;
}
