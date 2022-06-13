#pragma once

class AudioTest : public Component
{
private:

	virtual void Awake() override;
	virtual void Update() override;

private:

	AudioSource* m_audioSource;
	MeshRenderer* m_meshRenderer;
};

