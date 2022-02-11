#pragma once

ENGINE_BEGIN
class ResourceRefData;
class ENGINE_API IResourceObject abstract
{
public:

	virtual ~IResourceObject() = default;

	virtual void SetToDestroyed() = 0;

	virtual const ResourceRefData* GetRefData() const = 0;
};

ENGINE_END
