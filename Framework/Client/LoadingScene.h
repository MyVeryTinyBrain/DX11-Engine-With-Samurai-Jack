#pragma once

class LoadingScene : public Scene
{
	virtual Scene* Clone() override;
	virtual void OnLoad() override;
	virtual void OnUnload() override;
	virtual void OnUpdate() override;
	virtual void OnLateUpdate() override;

private:

	bool m_resourceLoaded = false;
};

