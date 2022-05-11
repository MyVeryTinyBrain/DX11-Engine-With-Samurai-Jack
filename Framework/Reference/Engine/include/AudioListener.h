#pragma once

#include "Component.h"
#include "SoundListener.h"

ENGINE_BEGIN

class ENGINE_API AudioListener : public Component
{
private:

	virtual void LateUpdate() override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;

private:

	Com<SoundListener>	m_listener;
};

ENGINE_END
