#include "GAS/Abilities/HeroAbilities/KRGA_EnterCombat.h"
#include "Equipment/KREquipmentManagerComponent.h"
#include "Components/KRCombatComponent.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "Characters/KRBaseCharacter.h"
#include "GameplayTag/KRPlayerTag.h"
#include "Item/Weapons/KRWeaponBase.h"
#include "Item/Weapons/KRMeleeWeapon.h"
#include "Item/Weapons/KRRangeWeapon.h"

UKRGA_EnterCombat::UKRGA_EnterCombat(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UKRGA_EnterCombat::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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

	AKRRangeWeapon* RangeWeapon = EquipComp->GetRangeActorInstance();
	if (RangeWeapon)
	{
		if (AKRMeleeWeapon* MeleeWeapon = EquipComp->GetMeleeActorInstance())
		{
			MeleeWeapon->PlayUnequipEffect();
		}

		RangeWeapon->PlayEquipEffect();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EnterCombat: Gun instance not found via EquipmentManager!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	KRASC->RemoveLooseGameplayTag(KRTAG_PLAYER_MODE_BASE);
	KRASC->RemoveLooseGameplayTag(KRTAG_PLAYER_MODE_SWORD);

	KRASC->AddLooseGameplayTag(KRTAG_PLAYER_MODE_GUN);
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UKRGA_EnterCombat::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}