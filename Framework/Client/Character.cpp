#include "stdafx.h"
#include "Character.h"

void Character::Awake()
{
	SetupCharacterController();
}

void Character::RotateOnYAxisToDirection(const V3& direction, float dt)
{
	if (direction.magnitude > 0.0f)
	{
		V3 up = V3(0, 1, 0);
		Q q = Q::LookRotation(direction.normalized, up);
		transform->rotation = Q::RotateTowards(transform->rotation, q, 720.0f * dt);
	}
}

void Character::SetupCharacterController()
{
	m_controller = gameObject->AddComponent<CharacterController>();
	m_controller->gravityScale = 2.0f;
}
