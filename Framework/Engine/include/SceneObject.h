#pragma once

#include "Object.h"

ENGINE_BEGIN
class ENGINE_API SceneObject abstract : public Object
{
public:

	SceneObject();

	SceneObject(const tstring& name);

	virtual ~SceneObject() = default;

protected:

	virtual void OnDestroy() {}

public:

	bool IsShouldDestroy() const;

	void Destroy();

private:

	bool m_shouldDestroy = false;

public:

	delegate<void(SceneObject*)> OnDestroyedCallback;

};
ENGINE_END

