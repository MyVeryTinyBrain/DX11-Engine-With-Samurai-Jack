#pragma once
#ifndef __SUB_RESOURCE_OBJECT_H__
#define __SUB_RESOURCE_OBJECT_H__

#include "Object.h"
#include "ISubResourceObject.h"
#include "ResourceRef.h"
#include "SubResourceRef.h"

// ���ҽ� ������Ʈ�� ���� ���ҽ��Դϴ�.
// �� ��ü�� ����� �Ǵ� ���ҽ��� �Ҹ�� �� �ݵ�� ���� �Ҹ�Ǿ�� �մϴ�.
// ���� ����� �Ǵ� ���ҽ��� ResourceRef<T> �� ���� ������ �� �����ϴ�. (���ҽ� ī��Ʈ�� �����˴ϴ�.)

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
