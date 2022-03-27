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
