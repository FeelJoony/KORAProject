#include "Enemy/KRAIC_Enemy.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISenseConfig_Sight.h"

void AKRAIC_Enemy::BeginPlay()
{
    Super::BeginPlay();

    SetLoseTargetDistance();
}

void AKRAIC_Enemy::SetLoseTargetDistance()
{
    if (UAIPerceptionComponent* Perception = FindComponentByClass<UAIPerceptionComponent>())
    {
        if (UAISenseConfig_Sight* Sight = Perception->GetSenseConfig<UAISenseConfig_Sight>())
        {
            LoseDistance = Sight->LoseSightRadius;
        }
    }
}

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

void AKRAIC_Enemy::SetAttackTarget(AActor* Target)
{
	AttackTarget = Target;
}
