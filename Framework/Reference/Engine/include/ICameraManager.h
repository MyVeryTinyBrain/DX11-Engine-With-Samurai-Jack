#pragma once

ENGINE_BEGIN

class ICamera;

class ENGINE_API ICameraManager abstract
{
public:

	virtual void AddCamera(ICamera* camera) = 0;

	virtual void RemoveCamera(ICamera* camera) = 0;

	virtual void SortByOrder() = 0;

	virtual void OnResolutionChanged(float width, float height, float aspect) = 0;
};

ENGINE_END

