#pragma once

#include "ISerializableLight.h"

EDITOR_BEGIN

class SerializableSpotLight : public ISerializableLight
{
	virtual Light* InitLight() override;
	virtual string GetType() const override;

	virtual void OnImGui() override;
	virtual void OnSerialize(Json::Value& json) const override;
	virtual void OnDeserialize(const Json::Value& json) override;
	virtual void OnDeserializeInRuntime(const Json::Value& json) override;

private:

	SpotLight* m_spotLight;
};

EDITOR_END
