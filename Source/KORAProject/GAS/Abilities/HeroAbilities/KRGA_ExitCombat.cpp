#include "GAS/Abilities/HeroAbilities/KRGA_ExitCombat.h"
#include "Equipment/KREquipmentManagerComponent.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "Characters/KRBaseCharacter.h"
#include "Item/Weapons/KRMeleeWeapon.h"
#include "Item/Weapons/KRRangeWeapon.h"
#include "GameplayTag/KRPlayerTag.h"

UKRGA_ExitCombat::UKRGA_ExitCombat(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UKRGA_ExitCombat::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                       const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	AKRBaseCharacter* Character = Cast<AKRBaseCharacter>(ActorInfo->AvatarActor.Get());
	UKREquipmentManagerComponent* EquipComp = Character ? Character->FindComponentByClass<UKREquipmentManagerComponent>() : nullptr;
	UKRAbilitySystemComponent* KRASC = GetKRAbilitySystemComponentFromActorInfo();

	if (!Character || !EquipComp || !KRASC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (AKRMeleeWeapon* MeleeWeapon = EquipComp->GetMeleeActorInstance())
	{
		MeleeWeapon->PlayUnequipEffect();
	}

	if (AKRRangeWeapon* RangeWeapon = EquipComp->GetRangeActorInstance())
	{
		RangeWeapon->PlayUnequipEffect();
	}
	
	KRASC->RemoveLooseGameplayTag(KRTAG_PLAYER_MODE_SWORD);
	KRASC->RemoveLooseGameplayTag(KRTAG_PLAYER_MODE_GUN);

	if (!KRASC->HasMatchingGameplayTag(KRTAG_PLAYER_MODE_BASE))
	{
		KRASC->AddLooseGameplayTag(KRTAG_PLAYER_MODE_BASE);
	}
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UKRGA_ExitCombat::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
