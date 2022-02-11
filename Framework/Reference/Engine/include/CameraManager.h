#pragma once

#include "ICameraManager.h"

ENGINE_BEGIN
class ENGINE_API CameraManager : public ICameraManager
{
public:

	ICamera* GetCamera(size_t index) const;

	size_t GetCameraCount() const;

	void SetMainCamera(ICamera* camera);

	ICamera* GetMainCamera() const;

	_declspec(property(get = GetMainCamera, put = SetMainCamera)) ICamera* mainCamera;

private:

	virtual void AddCamera(ICamera* camera) override;

	virtual void RemoveCamera(ICamera* camera) override;

	virtual void SortByOrder() override;

	virtual void OnResolutionChanged(float width, float height, float aspect) override;

private:

	bool Contains(ICamera* camera);

private:

	ICamera* m_mainCamera = nullptr;

	vector<ICamera*> m_cameras;
};

ENGINE_END
