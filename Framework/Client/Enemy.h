#pragma once

#include "Character.h"

class Player;

class Enemy : public Character
{
protected:

	virtual void Awake() override;
	virtual void OnDestroyed() override;

protected:

	Player* GetPlayer() const;
	V3 ToPlayerDelta() const;
	V3 ToPlayerDirection() const;
	V3 ToPlayerDirectionXZ() const;
	float DistanceBetweenPlayer() const;
	float XZDistanceBetweenPlayer() const;
	float XZAngleBetweenPlayer() const;

	_declspec(property(get = GetPlayer)) Player* player;

private:

	static vector<Enemy*> g_enemies;

protected:

	static void RegistEnemy(Enemy* enemy);
	static void UnregistEnemy(Enemy* enemy);

public:

	static inline uint GetEnemyCount() { return (uint)g_enemies.size(); }
	static inline Enemy* GetEnemyByIndex(uint index) { return g_enemies[index]; }
};

