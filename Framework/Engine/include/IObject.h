#pragma once

ENGINE_BEGIN
class RefData;
class ENGINE_API IObject
{
public:

	virtual ~IObject() = default;

	virtual void SetToDestroyed() = 0;

	virtual const RefData* GetRefData() const = 0;
};

ENGINE_END
