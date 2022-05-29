#pragma once

ENGINE_BEGIN

class ICamera;
struct RenderRequest;

class ENGINE_API IOnCamera abstract
{
public:

	virtual ~IOnCamera() = default;

	virtual void OnCamera(ICamera* camera, RenderRequest* inout_pinput) = 0;
};

ENGINE_END
