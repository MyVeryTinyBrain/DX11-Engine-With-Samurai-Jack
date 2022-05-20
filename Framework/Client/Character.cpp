#include "stdafx.h"
#include "Character.h"

void Character::Awake()
{
	SetupCharacterController();
}

void Character::SetupCharacterController()
{
	m_controller = gameObject->AddComponent<CharacterController>();
	m_controller->gravityScale = 2.0f;
}
