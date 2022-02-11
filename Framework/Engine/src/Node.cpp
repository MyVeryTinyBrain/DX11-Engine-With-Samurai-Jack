#include "EnginePCH.h"
#include "Node.h"

Node::Node(const TCHAR* name, bool hasBone, const M4& offsetMatrix, const M4& transformationMatirx, uint index, Node* parent, uint depth) :
	m_hasBone(hasBone),
	m_offsetMatrix(offsetMatrix),
	m_transformationMatrix(transformationMatirx),
	m_index(index),
	m_parent(parent),
	m_depth(depth)
{
	_tcscpy_s(m_name, name);

	if (parent)
	{
		parent->m_childs.push_back(this);

		m_defaultCombinedMatrix = m_transformationMatrix * parent->m_defaultCombinedMatrix;
	}
	else
	{
		m_defaultCombinedMatrix = m_transformationMatrix;
	}
}
