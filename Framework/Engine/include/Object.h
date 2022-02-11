#pragma once

#include "ObjectBase.h"
#include "IObject.h"

ENGINE_BEGIN
class RefData;
class ENGINE_API Object abstract : public ObjectBase, public IObject
{
	template <class T> friend class Ref;

public:

	Object();

	Object(const tstring& name);

	virtual ~Object();

private:

	virtual void SetToDestroyed() override;

	virtual const RefData* GetRefData() const override;

private:

	RefData* m_refData = nullptr;
};
ENGINE_END