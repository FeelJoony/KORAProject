#include "GAS/Abilities/KRHeroGameplayAbility.h"
#include "Characters/KRHeroCharacter.h"
#include "Controllers/KRHeroController.h"
#include "GAS/KRPlayerAbilitySystemComponent.h"

AKRHeroCharacter* UKRHeroGameplayAbility::GetHeroCharacterFromActorInfo()
{
	if (!CachedKRHeroCharacter.IsValid())
	{
		CachedKRHeroCharacter = Cast<AKRHeroCharacter>(CurrentActorInfo->AvatarActor);
	}
	
	return CachedKRHeroCharacter.IsValid() ? CachedKRHeroCharacter.Get() : nullptr;
}

AKRHeroController* UKRHeroGameplayAbility::GetHeroControllerFromActorInfo()
{
	if (!CachedKRHeroController.IsValid())
	{
		CachedKRHeroController = Cast<AKRHeroController>(CurrentActorInfo->PlayerController);
	}

	return CachedKRHeroController.IsValid() ? CachedKRHeroController.Get() : nullptr;
}

UHeroCombatComponent* UKRHeroGameplayAbility::GetHeroCombatComponentFromActorInfo()
{
	return GetHeroCharacterFromActorInfo()->GetHeroCombatComponent();
}

UKRPlayerAbilitySystemComponent* UKRHeroGameplayAbility::GetKRPlayerAbilitySystemComponentFromActorInfo() const
{
	return Cast<UKRPlayerAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
}
