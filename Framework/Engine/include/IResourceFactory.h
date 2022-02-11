#pragma once

ENGINE_BEGIN
class ResourceManagement;
class ENGINE_API IResourceFactory abstract
{
public:

	virtual ~IResourceFactory() = default;

	virtual HRESULT Initialize(ResourceManagement* management) = 0;
};

ENGINE_END
