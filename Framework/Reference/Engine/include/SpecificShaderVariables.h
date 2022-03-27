#pragma once
#include "ISpecificShaderVariable.h"

ENGINE_BEGIN

class SSVTime : public ISpecificShaderVariable // TIME
{
public:
	inline SSVTime(Material* material, const ShaderVariableInfo* shaderVariableInfo) : ISpecificShaderVariable(material, shaderVariableInfo) {};
	virtual void Apply(ICamera* camera) override;
};

class SSVUnscaledTime : public ISpecificShaderVariable // UNSCALED_TIME
{
public:
	inline SSVUnscaledTime(Material* material, const ShaderVariableInfo* shaderVariableInfo) : ISpecificShaderVariable(material, shaderVariableInfo) {};
	virtual void Apply(ICamera* camera) override;
};

class SSVDeltaTime : public ISpecificShaderVariable // DELTA_TIME
{
public:
	inline SSVDeltaTime(Material* material, const ShaderVariableInfo* shaderVariableInfo) : ISpecificShaderVariable(material, shaderVariableInfo) {};
	virtual void Apply(ICamera* camera) override;
};

class SSVFixedDeltaTime : public ISpecificShaderVariable // FIXED_DELTA_TIME
{
public:
	inline SSVFixedDeltaTime(Material* material, const ShaderVariableInfo* shaderVariableInfo) : ISpecificShaderVariable(material, shaderVariableInfo) {};
	virtual void Apply(ICamera* camera) override;
};

class SSVGrabTexture : public ISpecificShaderVariable // GRAB_TEXTURE
{
public:
	inline SSVGrabTexture(Material* material, const ShaderVariableInfo* shaderVariableInfo) : ISpecificShaderVariable(material, shaderVariableInfo) {};
	virtual void Apply(ICamera* camera) override;
};

class SSVDepthTexture : public ISpecificShaderVariable // DEPTH_TEXTURE
{
public:
	inline SSVDepthTexture(Material* material, const ShaderVariableInfo* shaderVariableInfo) : ISpecificShaderVariable(material, shaderVariableInfo) {};
	virtual void Apply(ICamera* camera) override;
};

ENGINE_END
