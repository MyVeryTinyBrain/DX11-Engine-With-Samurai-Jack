#pragma once

ENGINE_BEGIN
class ResourceObject;
class ResourceRefData;
class ENGINE_API ISubResourceObject abstract
{
public:

	virtual ~ISubResourceObject() = default;

	virtual ResourceObject* GetBasedResourceObjectPointer() const = 0;

};

ENGINE_END

