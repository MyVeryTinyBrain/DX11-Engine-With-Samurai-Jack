#pragma once

class Character abstract : public Component
{
protected:

	virtual void Awake() override;

public:

	inline CharacterController* GetCCT() const { return m_controller; }

	_declspec(property(get = GetCCT)) CharacterController* CCT;

private:

	void SetupCharacterController();

private:

	CharacterController* m_controller;
};

