#pragma once

#include "ShaderType.h"

ENGINE_BEGIN

class ShaderVariableInfo
{
public:

	struct Annotation
	{
		enum class	Type { Unknown, Scalar, Text, Vector };
		Type		type;
		string		name;
		float		scalar;
		string		text;
		V4			vector;
	};

private:

	ShaderVariableInfo(D3DX11_EFFECT_VARIABLE_DESC variableDesc, D3DX11_EFFECT_TYPE_DESC typeDesc, ID3DX11EffectVariable* handle);

public:

	static ShaderVariableInfo* Create(ID3DX11EffectVariable* handle);

public:

	inline ShaderVariableInfo(const ShaderVariableInfo& rhs) { *this = rhs; };
	inline ShaderVariableInfo(ShaderVariableInfo&& rhs) noexcept { *this = rhs; }

	~ShaderVariableInfo();

	ShaderVariableInfo& operator=(const ShaderVariableInfo& rhs);
	ShaderVariableInfo& operator=(ShaderVariableInfo&& rhs) noexcept;

	bool IsTexture() const;
	inline bool IsRawValue() const { return !IsTexture(); }
	inline bool IsArray() const { return (m_elements > 0); }

public:

	const string& GetName() const { return m_name; }
	const string& GetSemantic() const { return m_semantic; }
	const ShaderVariableType& GetType() const { return m_type; }
	const size_t& GetSize() const { return m_size; }
	const uint& GetElements() const { return m_elements; }
	uint GetArrayCount() const { return Clamp(m_elements, 1u, UINT_MAX); }
	ID3DX11EffectVariable* GetHandle() const { return m_handle; }
	const uint GetAnnotationCount() const { return (uint)m_annotations.size(); }
	const Annotation& GetAnnotation(uint index) const { return m_annotations[index]; }

	_declspec(property(get = GetName)) const string& Name;
	_declspec(property(get = GetSemantic)) const string& Semantic;
	_declspec(property(get = GetType)) const ShaderVariableType& Type;
	_declspec(property(get = GetSize)) const size_t& Size;
	_declspec(property(get = GetElements)) const uint& Elements;
	_declspec(property(get = GetArrayCount)) uint ArrayCount;
	_declspec(property(get = GetHandle)) ID3DX11EffectVariable* Handle;
	_declspec(property(get = GetAnnotationCount)) uint annotationCount;

private:

	void SetupAnnotations(D3DX11_EFFECT_VARIABLE_DESC variableDesc);

private:

	static ShaderVariableType TypeOf(const D3DX11_EFFECT_TYPE_DESC& typeDesc);

private:

	string						m_name;
	string						m_semantic;
	ShaderVariableType			m_type;
	size_t						m_size;
	uint						m_elements;
	ID3DX11EffectVariable*		m_handle;
	vector<Annotation>			m_annotations;
};

ENGINE_END

