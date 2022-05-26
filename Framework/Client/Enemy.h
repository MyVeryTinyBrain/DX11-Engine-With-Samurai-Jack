#pragma once

#include "Character.h"

class Player;

class Enemy : public Character
{
protected:

	virtual void Awake() override;

protected:

	Player* GetPlayer() const;
	V3 ToPlayerDelta() const;
	V3 ToPlayerDirection() const;
	V3 ToPlayerDirectionXZ() const;
	float DistanceBetweenPlayer() const;
	float XZDistanceBetweenPlayer() const;
	float XZAngleBetweenPlayer() const;

	_declspec(property(get = GetPlayer)) Player* player;
};

