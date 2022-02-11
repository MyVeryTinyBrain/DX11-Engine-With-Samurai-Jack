#include "EnginePCH.h"
#include "SubResourceObject.h"

SubResourceObject::SubResourceObject(const tstring& name, ResourceObject* basedResourceObject) :
	Object(name),
	m_basedResourceObject(basedResourceObject)
{
}
