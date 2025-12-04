#include "GAS/Abilities/HeroAbilities/KRGA_WeaponSwap.h"
#include "Equipment/KREquipmentManagerComponent.h"
#include "Weapons/KRWeaponInstance.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "Characters/KRBaseCharacter.h"
#include "Player/KRPlayerController.h"
#include "GameplayTag/KRItemTypeTag.h"
#include "GameplayTag/KRPlayerTag.h"

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
	AKRPlayerController* PC = Cast<AKRPlayerController>(ActorInfo->PlayerController.Get());
	UKREquipmentManagerComponent* EquipComp = Character ? Character->FindComponentByClass<UKREquipmentManagerComponent>() : nullptr;
	UKRAbilitySystemComponent* KRASC = GetKRAbilitySystemComponentFromActorInfo();

	if (!Character || !PC || !EquipComp || !KRASC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	FGameplayTag SwordTag = FGameplayTag::RequestGameplayTag("ItemType.Equip.Sword");
	FGameplayTag GunTag = FGameplayTag::RequestGameplayTag("ItemType.Equip.Gun");

	UKRWeaponInstance* SwordInst = nullptr;
	UKRWeaponInstance* GunInst = nullptr;

	TArray<UKREquipmentInstance*> AllWeapons = EquipComp->GetEquipmentInstancesOfType(UKRWeaponInstance::StaticClass());
	for (UKREquipmentInstance* Equip : AllWeapons)
	{
		if (UKRWeaponInstance* Weapon = Cast<UKRWeaponInstance>(Equip))
		{
			if (Weapon->WeaponType.MatchesTag(SwordTag))
			{
				SwordInst = Weapon;
			}
			else if (Weapon->WeaponType.MatchesTag(GunTag))
			{
				GunInst = Weapon;
			}
		}
	}

	if (KRASC->HasMatchingGameplayTag(KRTAG_PLAYER_MODE_SWORD))
	{
		if (GunInst)
		{
			if (SwordInst)
			{
				SwordInst->RemoveWeaponAbilities(KRASC);
				SwordInst->RemoveWeaponInputContext(PC);
			}

			GunInst->GrantWeaponAbilities(KRASC);
			GunInst->ApplyWeaponAnimLayer(Character);
			GunInst->AddWeaponInputContext(PC);

			KRASC->RemoveLooseGameplayTag(KRTAG_PLAYER_MODE_SWORD);
			KRASC->AddLooseGameplayTag(KRTAG_PLAYER_MODE_GUN);
		}
	}
	else if (KRASC->HasMatchingGameplayTag(KRTAG_PLAYER_MODE_GUN))
	{
		if (SwordInst)
		{
			if (GunInst)
			{
				GunInst->RemoveWeaponAbilities(KRASC);
				GunInst->RemoveWeaponInputContext(PC);
			}

			SwordInst->GrantWeaponAbilities(KRASC);
			SwordInst->ApplyWeaponAnimLayer(Character);
			SwordInst->AddWeaponInputContext(PC);

			KRASC->RemoveLooseGameplayTag(KRTAG_PLAYER_MODE_GUN);
			KRASC->AddLooseGameplayTag(KRTAG_PLAYER_MODE_SWORD);
		}
	}
	else
	{
		if (SwordInst)
		{
			SwordInst->GrantWeaponAbilities(KRASC);
			SwordInst->ApplyWeaponAnimLayer(Character);
			SwordInst->AddWeaponInputContext(PC);

			KRASC->RemoveLooseGameplayTag(KRTAG_PLAYER_MODE_BASE);
			KRASC->AddLooseGameplayTag(KRTAG_PLAYER_MODE_SWORD);
		}
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
