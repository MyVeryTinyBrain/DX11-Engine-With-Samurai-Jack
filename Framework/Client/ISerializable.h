#pragma once

EDITOR_BEGIN

class ISerializable abstract
{
public:

	virtual Json::Value Serialize() const = 0;
	virtual void Deserialize(const Json::Value& json) = 0;
	virtual void DeserializeInRuntime(const Json::Value& json) = 0;
};

EDITOR_END
