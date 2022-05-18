#pragma once

#include "Object.h"

ENGINE_BEGIN

class ENGINE_API AnimatorProperty : public Object
{
public:

	struct Value
	{
		AnimatorProperty::Value() : valueFloat(0) {}
		AnimatorProperty::Value(bool value) : valueBool(value) {}
		AnimatorProperty::Value(int value) : valueInt(value) {}
		AnimatorProperty::Value(float value) : valueFloat(value) {}
		union
		{
			bool	valueBool;
			int		valueInt;
			float	valueFloat;
		};
	};

	enum class Type { TRIGGER, BOOL, INT, FLOAT };

public:

	/// <summary>
	/// �ִϸ����� ������Ƽ�� �������Դϴ�.
	/// </summary>
	/// <param name="name">������Ƽ Ž���� ���� �̸��Դϴ�.</param>
	/// <param name="type">������Ƽ�� ������ ������ Ÿ���Դϴ�.</param>
	AnimatorProperty(const tstring& name, AnimatorProperty::Type type);

	virtual ~AnimatorProperty();

public:

	inline bool GetTriggerState() const { return m_value.valueBool; }
	inline bool GetValueAsBool() const { return m_value.valueBool; }
	inline int GetValueAsInt() const { return m_value.valueInt; }
	inline float GetValueAsFloat() const { return m_value.valueFloat; }

	inline void SetTriggerState() { m_value.valueBool = true; }
	inline void OffTriggerState() { m_value.valueBool = false; }
	inline void SetValueAsBool(bool value) { m_value.valueBool = value; }
	inline void SetValueAsInt(int value) { m_value.valueInt = value; }
	inline void SetValueAsFloat(float value) { m_value.valueFloat = value; }

	inline AnimatorProperty::Value GetValue() const { return m_value; }
	inline AnimatorProperty::Type GetType() const { return m_type; }

	_declspec(property(get = GetTriggerState)) bool triggerState;
	_declspec(property(get = GetValueAsBool, put = SetValueAsBool)) bool valueAsBool;
	_declspec(property(get = GetValueAsInt, put = SetValueAsInt)) int valueAsInt;
	_declspec(property(get = GetValueAsFloat, put = SetValueAsFloat)) float valueAsFloat;
	_declspec(property(get = GetValue)) AnimatorProperty::Value value;
	_declspec(property(get = GetType)) AnimatorProperty::Type type;

private:

	AnimatorProperty::Value		m_value;

	AnimatorProperty::Type		m_type;
};

ENGINE_END
