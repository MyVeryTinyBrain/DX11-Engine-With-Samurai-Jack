#include "stdafx.h"
#include "Character.h"

void Character::Awake()
{
	SetupCharacterController();
}

DamageOutType Character::Damage(const DamageIn& in)
{
	DamageOut out = {};
	out.In = in;

	out.Cosine = V3::Dot(GetDirection(), -in.FromDirection);

	if (out.Cosine < 0.0f)
	{
		out.Backattack = true;
	}

	enum { Guard_None, Guard_OK, Guard_Break };
	int guardResult = Guard_None;

	if (IsGuarding())
	{
		if (in.Guardable)
		{
			if (IsGuardBreakableByBackattack())
			{
				if (!out.Backattack)
					guardResult = Guard_OK;
			}
			else
			{
				guardResult = Guard_OK;
			}
		}
		else
		{
			if (IsGuardBreakableByBackattack())
			{
				if (!out.Backattack)
					guardResult = Guard_Break;
			}
		}
	}

	if (out.In.Type == DamageInType::NONE) 
	{
		return DamageOutType::FAILED;
	}
	else if (IsInvisible()) 
	{
		out.Type = DamageOutType::IGNORED;
	}
	else if (IsSuperarmor()) 
	{
		out.Type = DamageOutType::SUPERARMORED;
	}
	else if (guardResult == Guard_OK)
	{
		out.Type = DamageOutType::GUARDED;
	}
	else if (guardResult == Guard_Break)
	{
		out.Type = DamageOutType::GUARD_BREAKED;
	}
	else
	{
		out.Type = DamageOutType::HIT;
	}

	return OnDamage(out);
}

void Character::RotateOnYAxisToDirection(const V3& direction, float anglePerSec, float dt)
{
	if (direction.magnitude > 0.0f)
	{
		V3 up = V3(0, 1, 0);
		Q q = Q::LookRotation(direction.normalized, up);
		transform->rotation = Q::RotateTowards(transform->rotation, q, anglePerSec * dt);
	}
}

void Character::RotateOnYAxisToDirection(const V3& direction, float anglePerSec, float dt, float limitAngle)
{
	float a = V3::Angle(direction, GetDirection());
	if (a > limitAngle)
		return;

	RotateOnYAxisToDirection(direction, anglePerSec, dt);
}

void Character::SetupCharacterController()
{
	m_controller = gameObject->AddComponent<CharacterController>();
	m_controller->gravityScale = 2.0f;
}
