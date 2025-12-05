#include "GAS/Abilities/HeroAbilities/KRGA_EnterCombat.h"
#include "Equipment/KREquipmentManagerComponent.h"
#include "Weapons/KRWeaponInstance.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "Characters/KRBaseCharacter.h"
#include "Player/KRPlayerController.h"
#include "GameplayTag/KRPlayerTag.h"

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

	if (SwordInst)
	{
		SwordInst->ActivateWeapon(Character, PC, KRASC, EWeaponMessageAction::Equipped, true);
	}

	if (GunInst)
	{
		GunInst->DeactivateWeapon(Character, PC, KRASC, EWeaponMessageAction::Unequipped, false);
	}
	
	KRASC->RemoveLooseGameplayTag(KRTAG_PLAYER_MODE_BASE);
	KRASC->AddLooseGameplayTag(KRTAG_PLAYER_MODE_SWORD);
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
