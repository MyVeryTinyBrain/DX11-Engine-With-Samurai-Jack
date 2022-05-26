#pragma once

#include "IDamagable.h"

class Character abstract : public Component, public IDamagable
{
protected:

	virtual void Awake() override;

public:

	virtual float GetHP() const = 0;
	virtual void SetHP(float value) = 0;
	
	virtual V3 GetDirection() const = 0;
	virtual void SetDirection(const V3& direction) = 0;
	virtual bool IsGuarding() const = 0;
	virtual bool IsGuardBreakableByBackattack() const = 0;
	virtual bool IsInvisible() const = 0;
	virtual bool IsSuperarmor() const = 0;
	virtual bool IsDead() const { return GetHP() <= 0.0f; }

	_declspec(property(get = GetHP, put = SetHP)) float hp;
	_declspec(property(get = GetDirection, put = SetDirection)) V3 direction;
	_declspec(property(get = IsGuarding)) bool isGuarding;
	_declspec(property(get = IsGuardBreakableByBackattack)) bool isGuardBreakableByBackattack;
	_declspec(property(get = IsSuperarmor)) bool isSuperarmor;
	_declspec(property(get = IsDead)) bool isDead;

public:

	virtual DamageOutType Damage(const DamageIn& in) override;

protected:

	virtual DamageOutType OnDamage(const DamageOut& out) = 0;

public:

	inline CharacterController* GetCCT() const { return m_controller; }

	_declspec(property(get = GetCCT)) CharacterController* CCT;

protected:

	void RotateOnYAxisToDirection(const V3& direction, float anglePerSec, float dt);
	void RotateOnYAxisToDirection(const V3& direction, float anglePerSec, float dt, float limitAngle);

private:

	void SetupCharacterController();

private:

	CharacterController* m_controller;
};

