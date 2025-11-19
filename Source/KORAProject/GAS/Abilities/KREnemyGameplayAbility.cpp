#include "GAS/Abilities/KREnemyGameplayAbility.h"
#include "Characters/KREnemyCharacter.h"

AKREnemyCharacter* UKREnemyGameplayAbility::GetEnemyCharacterFromActorInfo()
{
	if (!CachedKREnemyCharacter.IsValid())
	{
		CachedKREnemyCharacter = Cast<AKREnemyCharacter>(CurrentActorInfo->AvatarActor);
	}

	return CachedKREnemyCharacter.IsValid() ? CachedKREnemyCharacter.Get() : nullptr;
}

UEnemyCombatComponent* UKREnemyGameplayAbility::GetEnemyCombatComponentFromActorInfo()
{
	return GetEnemyCharacterFromActorInfo()->GetEnemyCombatComponent();
}
