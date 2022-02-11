#include "EnginePCH.h"
#include "SceneObject.h"

SceneObject::SceneObject() : Object()
{
}

SceneObject::SceneObject(const tstring & name) : Object(name)
{
}

bool SceneObject::IsShouldDestroy() const
{
	return m_shouldDestroy;
}

void SceneObject::Destroy()
{
	if (m_shouldDestroy)
		return;

	m_shouldDestroy = true;

	IObject* iObject = this;
	iObject->SetToDestroyed();

	OnDestroy();
}
