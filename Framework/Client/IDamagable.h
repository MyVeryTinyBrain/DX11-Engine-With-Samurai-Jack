#pragma once

enum class DamageInType { NONE, LIGHT, HEAVY, BLOW, BLOWUP, BLOWDOWN };
enum class DamageOutType { FAILED, HIT, GUARDED, GUARD_BREAKED, IGNORED, SUPERARMORED };

struct DamageIn
{
	static const uint MAX_CONTEXT = 64;

	float			Damage;
	V3				FromDirection;
	DamageInType	Type;
	bool			Guardable;
	bool			SetVelocity;
	V3				Velocity;
	Component*		FromComponent; // Nullable
};

struct DamageOut
{
	bool			Backattack;
	float			Cosine;
	DamageOutType	Type;
	DamageIn		In;
};

class IDamagable abstract
{
public:
	virtual DamageOutType Damage(const DamageIn& in) = 0;
};

