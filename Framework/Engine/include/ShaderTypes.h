#pragma once

ENGINE_BEGIN

enum class RenderGroup 
{ 
	Priority, 
	Standard, 
	AlphaTest,
	Transparent, 
	Overlay, 
	Max 
};

enum class ShaderVariableType
{
	Unknown,
	Other,
	Struct,
	String,
	Bool,
	Int,
	UInt,
	Float,
	Vector,
	Matrix,
	Texture1D,
	Texture2D,
	Texture3D,
	Texture1DArray,
	Texture2DArray,
};

struct ShaderVariableInfo
{
	string						Name;
	string						Semantic;
	ShaderVariableType			Type;
	size_t						Size;
	uint						Elements;
	ID3DX11EffectVariable*		Handle;

	inline ShaderVariableInfo()
	{
		Type = ShaderVariableType(0);
		Size = 0;
		Elements = 0;
		Handle = 0;
	}

	inline ShaderVariableInfo(const ShaderVariableInfo& rhs)
	{
		*this = rhs;
	}

	inline ShaderVariableInfo(ShaderVariableInfo&& rhs) noexcept
	{
		*this = rhs;
	}

	inline ~ShaderVariableInfo()
	{
		SafeRelease(Handle);
	}

	inline ShaderVariableInfo& operator=(const ShaderVariableInfo& rhs)
	{
		Name = rhs.Name;
		Semantic = rhs.Semantic;
		Type = rhs.Type;
		Size = rhs.Size;
		Elements = rhs.Elements;
		Handle = rhs.Handle;
		rhs.Handle->AddRef();

		return *this;
	}

	inline ShaderVariableInfo& operator=(ShaderVariableInfo&& rhs) noexcept
	{
		Name = rhs.Name;
		Semantic = rhs.Semantic;
		Type = rhs.Type;
		Size = rhs.Size;
		Elements = rhs.Elements;
		Handle = rhs.Handle;

		rhs.Handle = nullptr;

		return *this;
	}

	inline bool IsArray() const
	{
		return (Elements > 0);
	}
};

struct ShaderVariableDesc
{
	ShaderVariableInfo		Info;
	void*					Pointer;
	Byte*					ValueBytes;
	size_t					ValueSize;

	inline ShaderVariableDesc()
	{
		Info = {};
		Pointer = 0;
		ValueBytes = 0;
		ValueSize = 0;
	}

	inline ShaderVariableDesc(size_t valueSize)
	{
		Info = {};
		Pointer = 0;
		ValueBytes = new Byte[valueSize];
		ValueSize = valueSize;
	}

	inline ShaderVariableDesc(const ShaderVariableDesc& rhs)
	{
		*this = rhs;
	}

	inline ShaderVariableDesc(ShaderVariableDesc&& rhs) noexcept
	{
		*this = rhs;
	}

	inline ~ShaderVariableDesc()
	{
		SafeDeleteArray(ValueBytes);
	}

	inline ShaderVariableDesc& operator=(const ShaderVariableDesc& rhs)
	{
		memcpy(&Info, &rhs.Info, sizeof(rhs.Info));
		Pointer = rhs.Pointer;
		memcpy(ValueBytes, rhs.ValueBytes, rhs.ValueSize);
		ValueSize = rhs.ValueSize;

		return *this;
	}

	inline ShaderVariableDesc& operator=(ShaderVariableDesc&& rhs) noexcept
	{
		memcpy(&Info, &rhs.Info, sizeof(rhs.Info));
		Pointer = rhs.Pointer;
		ValueBytes = rhs.ValueBytes;
		ValueSize = rhs.ValueSize;

		rhs.Info = {};
		rhs.Pointer = nullptr;
		rhs.ValueBytes = nullptr;
		rhs.ValueSize = 0;

		return *this;
	}

	inline bool IsArray() const
	{
		return Info.IsArray();
	}
};

ENGINE_END
