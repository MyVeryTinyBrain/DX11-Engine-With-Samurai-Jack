#pragma once

#include "Component.h"

ENGINE_BEGIN

class ENGINE_API FPSChecker : public Component
{
	virtual void Update() override;

private:

	float m_accumulated = 0;

	unsigned int m_fps = 0;
};

ENGINE_END
