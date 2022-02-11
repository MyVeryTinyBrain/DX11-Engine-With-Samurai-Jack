#include "EnginePCH.h"
#include "Object.h"
#include "RefData.h"

Object::Object() : ObjectBase()
{
	m_refData = new RefData(this);
}

Object::Object(const tstring& name) : ObjectBase(name)
{
	m_refData = new RefData(this);
}

Object::~Object()
{
	SetToDestroyed();

	SafeDelete(m_refData);
}

void Object::SetToDestroyed()
{
	m_refData->SetToDestroyed();
}

const RefData* Object::GetRefData() const
{
	return m_refData;
}
