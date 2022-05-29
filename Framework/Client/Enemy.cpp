#include "stdafx.h"
#include "Enemy.h"
#include "Player.h"
#include "Config.h"

vector<Enemy*> Enemy::g_enemies;

void Enemy::Awake()
{
    Character::Awake();
    CCT->capsuleCollider->layerIndex = PhysicsLayer_Enemy;

    RegistEnemy(this);
}

void Enemy::OnDestroyed()
{
    UnregistEnemy(this);
}

Player* Enemy::GetPlayer() const
{
    return Player::GetInstance();
}

V3 Enemy::ToPlayerDelta() const
{
    if (!player)
        return V3::zero();

    return player->transform->position - transform->position;
}

V3 Enemy::ToPlayerDirection() const
{
    return ToPlayerDelta().normalized;
}

V3 Enemy::ToPlayerDirectionXZ() const
{
    V3 dir = ToPlayerDirection();
    dir.y = 0.0f;
    return dir.normalized;
}

float Enemy::DistanceBetweenPlayer() const
{
    return ToPlayerDelta().magnitude;
}

float Enemy::XZDistanceBetweenPlayer() const
{
    V3 delta = ToPlayerDelta();
    delta.y = 0.0f;
    return delta.magnitude;
}

float Enemy::XZAngleBetweenPlayer() const
{
    V3 xzForard = transform->forward;
    xzForard.y = 0;
    xzForard.Normalize();
    return V3::Angle(ToPlayerDirectionXZ(), xzForard);
}

void Enemy::RegistEnemy(Enemy* enemy)
{
    auto find_it = std::find(g_enemies.begin(), g_enemies.end(), enemy);
    if (find_it == g_enemies.end())
    {
        g_enemies.push_back(enemy);
    }
}

void Enemy::UnregistEnemy(Enemy* enemy)
{
    auto find_it = std::find(g_enemies.begin(), g_enemies.end(), enemy);
    if (find_it != g_enemies.end())
    {
        g_enemies.erase(find_it);
    }
}
