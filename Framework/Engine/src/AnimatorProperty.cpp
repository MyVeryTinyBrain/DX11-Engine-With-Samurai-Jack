#include "EnginePCH.h"
#include "AnimatorProperty.h"

AnimatorProperty::AnimatorProperty(const tstring& name, AnimatorProperty::Type type) :
	Object(name),
	m_type(type)
{
	ZeroMemory(&m_value, sizeof(Value));
}

AnimatorProperty::~AnimatorProperty()
{
}
