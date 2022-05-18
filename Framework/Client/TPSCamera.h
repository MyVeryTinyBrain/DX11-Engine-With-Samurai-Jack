#pragma once

class TPSCamera : public Component
{
private:

	virtual void Awake() override;

	virtual void Start() override;

	virtual void Update() override;

public:

	inline const Ref<Transform>& GetTarget() const { return m_target; }
	inline void SetTarget(const Ref<Transform>& target) { m_target = target; }

	inline float GetMinDistance() const { return m_minDistance; }
	inline void SetMinDistance(float value) { m_minDistance = Clamp(value, 0.0f, m_minDistance); }
	
	inline float GetMaxDistance() const { return m_maxDistance; }
	inline void SetMaxDistance(float value) { m_maxDistance = Clamp(value, m_minDistance, FLT_MAX); }

	inline float GetDistance() const { return m_distance; }
	inline void SetDistance(float value) { m_distance = Clamp(value, m_minDistance, m_maxDistance); }

	inline Camera* GetCamera() const { return m_camera; }

	_declspec(property(get = GetTarget, put = SetTarget)) const Ref<Transform>& target;
	_declspec(property(get = GetMinDistance, put = SetMinDistance)) float minDistance;
	_declspec(property(get = GetMaxDistance, put = SetMaxDistance)) float maxDistance;
	_declspec(property(get = GetDistance, put = SetDistance)) float distance;
	_declspec(property(get = GetCamera)) Camera* camera;

private:

	void UpdateRotation();

	void UpdateDistance();

	void UpdatePosition();

private:

	V2 DeltaCursor() const;

	void SetCursorToCenter();

private:

	Ref<Transform>	m_target;

	GameObject*		m_goCamera;
	Camera*			m_camera;

	float			m_minDistance = 1.0f;
	float			m_maxDistance = 10.0f;
	float			m_distance = 4.0f;
	float			m_height = 1.0f;
	float			m_tracePower = 5.0f;

	float			m_rotationSpeed = 0.1f;

	bool			m_rotation = true;
};

