#include "GAS/Abilities/HeroAbilities/KRGA_WeaponSwap.h"
#include "Equipment/KREquipmentManagerComponent.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "Characters/KRBaseCharacter.h"
#include "Player/KRPlayerController.h"
#include "GameplayTag/KRItemTypeTag.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayTag/KRPlayerTag.h"
#include "Item/Weapons/KRMeleeWeapon.h"
#include "Item/Weapons/KRRangeWeapon.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"

UKRGA_WeaponSwap::UKRGA_WeaponSwap(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;	
}

void UKRGA_WeaponSwap::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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

	AKRMeleeWeapon* MeleeWeapon = EquipComp->GetMeleeActorInstance();
	AKRRangeWeapon* RangeWeapon = EquipComp->GetRangeActorInstance();

	if (KRASC->HasMatchingGameplayTag(KRTAG_PLAYER_MODE_SWORD))
	{
		if (RangeWeapon)
		{
			if (MeleeWeapon)
			{
				MeleeWeapon->PlayUnequipEffect();
			}
			
			RangeWeapon->PlayEquipEffect();

			KRASC->RemoveLooseGameplayTag(KRTAG_PLAYER_MODE_SWORD);
			KRASC->AddLooseGameplayTag(KRTAG_PLAYER_MODE_GUN);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("WeaponSwap: Cannot switch to Gun (Not Equipped)"));
		}
	}
	else if (KRASC->HasMatchingGameplayTag(KRTAG_PLAYER_MODE_GUN))
	{
		if (MeleeWeapon)
		{
			if (RangeWeapon)
			{
				RangeWeapon->PlayUnequipEffect();
			}
			MeleeWeapon->PlayEquipEffect();

			KRASC->RemoveLooseGameplayTag(KRTAG_PLAYER_MODE_GUN);
			KRASC->AddLooseGameplayTag(KRTAG_PLAYER_MODE_SWORD);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("WeaponSwap: Cannot switch to Sword (Not Equipped)"));
		}
	}

	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UKRGA_WeaponSwap::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
