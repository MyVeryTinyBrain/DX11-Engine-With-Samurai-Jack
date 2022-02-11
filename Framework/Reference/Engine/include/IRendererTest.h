#pragma once

ENGINE_BEGIN
class ICamera;
class ENGINE_API IRendererTest abstract
{
public:

	virtual ~IRendererTest() = default;

	virtual void OnCamera(ICamera* camera) = 0;

	virtual bool CullTest(ICamera* camera) const = 0;
};

ENGINE_END
