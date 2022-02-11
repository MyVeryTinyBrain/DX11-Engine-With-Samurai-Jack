#pragma once
#ifndef __SUB_RESOURCE_OBJECT_H__
#define __SUB_RESOURCE_OBJECT_H__

#include "Object.h"
#include "ISubResourceObject.h"
#include "ResourceRef.h"
#include "SubResourceRef.h"

// 리소스 오브젝트의 보조 리소스입니다.
// 이 객체는 기반이 되는 리소스가 소멸될 때 반드시 같이 소멸되어야 합니다.
// 또한 기반이 되는 리소스를 ResourceRef<T> 를 통해 참조할 수 없습니다. (리소스 카운트가 문제됩니다.)

ENGINE_BEGIN

class ENGINE_API SubResourceObject abstract : public Object, public ISubResourceObject
{
public:

	SubResourceObject(const tstring& name, ResourceObject* basedResourceObject);

	virtual ~SubResourceObject() = default;

public:

	inline ResourceRef<ResourceObject> GetBasedResourceObject() const { return m_basedResourceObject; }

private:

	inline virtual ResourceObject* GetBasedResourceObjectPointer() const final override { return m_basedResourceObject; };

protected:

	ResourceObject* m_basedResourceObject = nullptr;
};

ENGINE_END
#endif
