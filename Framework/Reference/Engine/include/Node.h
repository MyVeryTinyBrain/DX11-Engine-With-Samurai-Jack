#pragma once

#include "NodeDesc.h"

ENGINE_BEGIN

class ENGINE_API Node
{
public:

	Node(const TCHAR* name, bool hasBone, const M4& offsetMatrix, const M4& transformationMatirx, uint index, Node* parent, uint depth);

	~Node() = default;

public:

	inline const TCHAR* GetName() const { return m_name; }

	inline const bool HasBone() const { return m_hasBone; }

	inline const M4& GetOffsetMatrix() const { return m_offsetMatrix; }

	inline const M4& GetTransformationMatrix() const { return m_transformationMatrix; }

	inline const M4& GetDefaultCombinedMatrix() const { return m_defaultCombinedMatrix; }

	inline uint GetIndex() const { return m_index; }

	inline const Node* GetParent() const { return m_parent; }

	inline size_t GetChildCount() const { return m_childs.size(); }

	inline const Node* GetChild(uint index) const { return m_childs[index]; }

	inline uint GetDepth() const { return m_depth; }

	_declspec(property(get = GetName)) const TCHAR* name;
	_declspec(property(get = HasBone)) bool hasBone;
	_declspec(property(get = GetOffsetMatrix)) const M4& offsetMatrix;
	_declspec(property(get = GetTransformationMatrix)) const M4& transformationMatrix;
	_declspec(property(get = GetIndex)) uint index;
	_declspec(property(get = GetParent)) const Node* parent;
	_declspec(property(get = GetChildCount)) size_t childCount;
	_declspec(property(get = GetDepth)) uint depth;

private:

	TCHAR				m_name[MAXLEN] = {};

	bool				m_hasBone = false;

	M4					m_offsetMatrix;

	M4					m_transformationMatrix;

	M4					m_defaultCombinedMatrix;

	uint				m_index;

	Node*				m_parent = nullptr;

	vector<Node*>		m_childs;

	uint				m_depth;
};

ENGINE_END
