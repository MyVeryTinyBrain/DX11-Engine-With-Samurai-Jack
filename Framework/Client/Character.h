#pragma once

class Character;

struct DamageDesc
{
	enum class Type { LIGHT, HEAVY, BLOW, BLOWUP, BLOWDOWN };
	static const uint MAX_CONTEXT = 64;

	float		Damage;
	V3			FromDirection;
	Type		Type;
	bool		Guardable;
	bool		IgnoreHitableFlag;
	bool		SetVelocity;
	V3			Velocity;

	Character*	FromCharacter; // Nullable
	CHAR		Context[MAX_CONTEXT]; // Nullable
};

enum class DamageResult { HIT, GUARDED, IGNORED, SUPERARMOR };

class Character abstract : public Component
{
protected:

	virtual void Awake() override;

public:

	virtual float GetHP() const { return 0.0f; }
	virtual void SetHP(float value) {}
	
	virtual bool IsHitable() const { return true; }
	virtual bool IsGuarding() const { return false; }
	virtual bool IsSuperarmor() const { return false; }

	_declspec(property(get = GetHP, put = SetHP)) float hp;
	_declspec(property(get = IsHitable)) bool isHitable;
	_declspec(property(get = IsGuarding)) bool isGuarding;
	_declspec(property(get = IsSuperarmor)) bool isSuperarmor;

public:

	virtual DamageResult Damage(const DamageDesc& desc) { return DamageResult::IGNORED; }

public:

	inline CharacterController* GetCCT() const { return m_controller; }

	_declspec(property(get = GetCCT)) CharacterController* CCT;

protected:

	void RotateOnYAxisToDirection(const V3& direction, float anglePerSec, float dt);

private:

	void SetupCharacterController();

private:

	CharacterController* m_controller;
};

