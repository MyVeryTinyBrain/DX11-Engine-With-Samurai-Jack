#pragma once

#include "EventListener.h"

class Enemy;

class TPSCamera : public Component, public EventListener
{
	struct Shake_t
	{
		V3 direction;
		float power;
		float duration;
		float shakePerSec;
		float square;
		float elapsed = 0.0f;
	};

private:

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void OnDestroyed() override;
	virtual void OnEvent(const string& msg, void* pContext) override;

public:

	inline const Ref<Transform>& GetTraceTarget() const { return m_traceTarget; }
	inline void SetTraceTarget(const Ref<Transform>& target) { m_traceTarget = target; }

	inline const Ref<Transform>& GetLookTarget() const { return m_lookTarget; }
	void SetLookTarget(const Ref<Transform>& target);

	inline float GetDistance() const { return m_distance; }
	inline void SetDistance(float value) { m_distance = Max(value, 0.0f); }

	inline Camera* GetCamera() const { return m_camera; }

	_declspec(property(get = GetTraceTarget, put = SetTraceTarget)) const Ref<Transform>& traceTarget;
	_declspec(property(get = GetLookTarget, put = SetLookTarget)) const Ref<Transform>& lookTarget;
	_declspec(property(get = GetMinDistance, put = SetMinDistance)) float minDistance;
	_declspec(property(get = GetMaxDistance, put = SetMaxDistance)) float maxDistance;
	_declspec(property(get = GetDistance, put = SetDistance)) float distance;
	_declspec(property(get = GetCamera)) Camera* camera;

public:

	void LookNearEnemy();
	void Unlook();
	void Shake(const V3& direction, float power, float square, float duration, float shakePerSec);

private:

	Enemy* FindNearEnemy() const;
	Enemy* FindOtherNearEnemy(Transform* selected, const V3& deltaCursor) const;

private:

	void UpdateRotation();
	void UpdateShake();
	void UpdatePosition();
	void UpdateDistance();

private:

	V2 DeltaCursor() const;
	void SetCursorToCenter();
	V3 CalcTargetPos() const;

private:

	Ref<Transform>	m_traceTarget;
	Ref<Transform>	m_lookTarget;

	// 실제 카메라가 부착되는 게임오브젝트입니다.
	// forward 방향의 반대쪽으로 distance 만큼 위치하게 됩니다.
	GameObject*		m_goCamera;
	Camera*			m_camera;

	float			m_distance = 5.0f;
	float			m_height = 0.75f;
	float			m_xztracePower = 5.0f;
	float			m_ytracePower = 10.0f;

	float			m_manualRotateSpeed = 0.1f;
	float			m_lookRotateSpeed = 0.25f;
	float			m_lookChangeDelay = 0.05f;
	float			m_lookChangeDelayCount = 0.0f;
	float			m_minLookChangeDelta = 20.0f;
	bool			m_rotate = true;

	V3				m_shakeTranslation;
	vector<Shake_t>	m_shakes;
};

