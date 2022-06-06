#pragma once

#include "ISerializableLight.h"

EDITOR_BEGIN

class SerializableDirectionalLight : public ISerializableLight
{
	virtual Light* InitLight() override;
	virtual string GetType() const override;

public:

	DirectionalLight* GetDirectionalLight() const { return m_directionalLight; }

private:

	DirectionalLight* m_directionalLight;
};

EDITOR_END
