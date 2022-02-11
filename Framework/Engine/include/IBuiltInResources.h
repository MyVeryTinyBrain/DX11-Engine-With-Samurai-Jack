#pragma once

ENGINE_BEGIN
class ResourceFactory;
class ENGINE_API IBuiltInResources abstract
{
public:

	virtual ~IBuiltInResources() = default;

	virtual HRESULT Initialize(ResourceFactory* factory) = 0;
};

ENGINE_END
