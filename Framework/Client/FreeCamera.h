#pragma once

class FreeCamera : public Component
{
private:

	virtual void Awake() override;

	virtual void Start() override;

	virtual void Update() override;

private:

	void UpdateTranslation();

	void UpdateRotation();

private:

	V2 DeltaCursor() const;

	void SetCursorToCenter();

public:

	inline Camera* GetCamera() const { return m_camera; }

	inline float GetTranslationSpeed() const { return m_translationSpeed; }
	inline void SetTranslationSpeed(float value) { m_translationSpeed = Clamp(value, 0.0f, FLT_MAX); }

	inline float GetTranslationFactor() const { return m_translationFactor; }
	inline void SetTranslationFactor(float value) { m_translationFactor = Clamp(value, 0.0f, FLT_MAX); }

	inline float GetRotationSpeed() const { return m_rotationSpeed; }
	inline void SetRotationSpeed(float value) { m_rotationSpeed = Clamp(value, 0.0f, FLT_MAX); }

	_declspec(property(get = GetCamera)) Camera* camera;
	_declspec(property(get = GetTranslationSpeed, put = SetTranslationSpeed)) float translationSpeed;
	_declspec(property(get = GetTranslationFactor, put = SetTranslationFactor)) float translationFactor;
	_declspec(property(get = GetRotationSpeed, put = SetRotationSpeed)) float rotationSpeed;

private:

	Camera*			m_camera;
	AudioListener*	m_listener;

	float			m_translationSpeed = 1.0f;
	float			m_translationFactor = 10.0f;
	float			m_rotationSpeed = 0.15f;

	bool			m_rotation = true;
};

