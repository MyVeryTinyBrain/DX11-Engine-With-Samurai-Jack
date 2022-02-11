#pragma once

#include "ILight.h"

ENGINE_BEGIN

class ENGINE_API ILightManager
{
public:

	virtual ~ILightManager() = default;

public:

	virtual bool AddLight(ILight* light) = 0;

	virtual bool RemoveLight(ILight* light) = 0;
};

ENGINE_END
