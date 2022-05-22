#pragma once

class Character;

struct DamageDesc
{
	enum class Type { LIGHT, HEAVY, BLOW, BLOWUP };
	static const uint MAX_CONTEXT = 64;

	float		Damage;
	V3			FromDirection;
	Type		Type;
	bool		Guardable;

	Character*	FromCharacter; // Nullable
	CHAR		Context[MAX_CONTEXT]; // Nullable
};

class Character abstract : public Component
{
protected:

	virtual void Awake() override;

public:

	virtual float GetHP() const { return 0.0f; }
	virtual void SetHP(float value) {}
	
	_declspec(property(get = GetHP, put = SetHP)) float hp;

public:

	virtual void Damage(const DamageDesc& desc) {}

public:

	inline CharacterController* GetCCT() const { return m_controller; }

	_declspec(property(get = GetCCT)) CharacterController* CCT;

protected:

	void RotateOnYAxisToDirection(const V3& direction, float dt);

private:

	void SetupCharacterController();

private:

	CharacterController* m_controller;
};

