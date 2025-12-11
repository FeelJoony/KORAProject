#include "Enemy/KRAIC_Enemy.h"

void AKRAIC_Enemy::Patrol() const
{
}

void AKRAIC_Enemy::Alert() const
{
}

void AKRAIC_Enemy::ChasePlayer() const
{
}

void AKRAIC_Enemy::SenseRange() const
{
	OnAlertEnd.Broadcast();
}
