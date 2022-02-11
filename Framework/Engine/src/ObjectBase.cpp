#include "EnginePCH.h"
#include "ObjectBase.h"

ObjectBase::ObjectBase() :
	m_name(TEXT(""))
{
}

ObjectBase::ObjectBase(const tstring& name) :
	m_name(name)
{
}

void ObjectBase::SetName(const tstring& name)
{
	m_name = name;
}

const tstring& ObjectBase::GetName() const
{
	return m_name;
}
