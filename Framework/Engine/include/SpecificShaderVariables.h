#pragma once
#include "ISpecificShaderVariable.h"

ENGINE_BEGIN

class SSVTime : public ISpecificShaderVariable // TIME
{
public:
	inline SSVTime(Material* material, const ShaderVariableInfo* shaderVariableInfo) : ISpecificShaderVariable(material, shaderVariableInfo) {};
	inline SSVTime(Material* material, const ISpecificShaderVariable& other) : ISpecificShaderVariable(material, other) {};
	virtual void Apply(ICamera* camera) override;
	inline virtual ISpecificShaderVariable* Copy(Material* material, const ISpecificShaderVariable& other) const override { return new SSVTime(*this); }
};

class SSVUnscaledTime : public ISpecificShaderVariable // UNSCALED_TIME
{
public:
	inline SSVUnscaledTime(Material* material, const ShaderVariableInfo* shaderVariableInfo) : ISpecificShaderVariable(material, shaderVariableInfo) {};
	inline SSVUnscaledTime(Material* material, const ISpecificShaderVariable& other) : ISpecificShaderVariable(material, other) {};
	virtual void Apply(ICamera* camera) override;
	inline virtual ISpecificShaderVariable* Copy(Material* material, const ISpecificShaderVariable& other) const override { return new SSVUnscaledTime(*this); }
};

class SSVDeltaTime : public ISpecificShaderVariable // DELTA_TIME
{
public:
	inline SSVDeltaTime(Material* material, const ShaderVariableInfo* shaderVariableInfo) : ISpecificShaderVariable(material, shaderVariableInfo) {};
	inline SSVDeltaTime(Material* material, const ISpecificShaderVariable& other) : ISpecificShaderVariable(material, other) {};
	virtual void Apply(ICamera* camera) override;
	virtual ISpecificShaderVariable* Copy(Material* material, const ISpecificShaderVariable& other) const override { return new SSVDeltaTime(*this); }
};

class SSVFixedDeltaTime : public ISpecificShaderVariable // FIXED_DELTA_TIME
{
public:
	inline SSVFixedDeltaTime(Material* material, const ShaderVariableInfo* shaderVariableInfo) : ISpecificShaderVariable(material, shaderVariableInfo) {};
	inline SSVFixedDeltaTime(Material* material, const ISpecificShaderVariable& other) : ISpecificShaderVariable(material, other) {};
	virtual void Apply(ICamera* camera) override;
	inline virtual ISpecificShaderVariable* Copy(Material* material, const ISpecificShaderVariable& other) const override { return new SSVFixedDeltaTime(*this); }
};

class SSVGrabTexture : public ISpecificShaderVariable // GRAB_TEXTURE
{
public:
	inline SSVGrabTexture(Material* material, const ShaderVariableInfo* shaderVariableInfo) : ISpecificShaderVariable(material, shaderVariableInfo) {};
	inline SSVGrabTexture(Material* material, const ISpecificShaderVariable& other) : ISpecificShaderVariable(material, other) {};
	virtual void Apply(ICamera* camera) override;
	inline virtual ISpecificShaderVariable* Copy(Material* material, const ISpecificShaderVariable& other) const override { return new SSVGrabTexture(*this); }
};

class SSVDepthTexture : public ISpecificShaderVariable // DEPTH_TEXTURE
{
public:
	inline SSVDepthTexture(Material* material, const ShaderVariableInfo* shaderVariableInfo) : ISpecificShaderVariable(material, shaderVariableInfo) {};
	inline SSVDepthTexture(Material* material, const ISpecificShaderVariable& other) : ISpecificShaderVariable(material, other) {};
	virtual void Apply(ICamera* camera) override;
	inline virtual ISpecificShaderVariable* Copy(Material* material, const ISpecificShaderVariable& other) const override { return new SSVDepthTexture(*this); }
};

ENGINE_END
