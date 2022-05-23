#include "stdafx.h"
#include "Enemy.h"
#include "Player.h"
#include "Config.h"

void Enemy::Awake()
{
    Character::Awake();
    CCT->capsuleCollider->layerIndex = PhysicsLayer_Enemy;
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
