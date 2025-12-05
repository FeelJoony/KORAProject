#include "GAS/Abilities/HeroAbilities/KRGA_ExitCombat.h"
#include "Equipment/KREquipmentManagerComponent.h"
#include "Weapons/KRWeaponInstance.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "Characters/KRBaseCharacter.h"
#include "Player/KRPlayerController.h"
#include "GameplayTag/KRPlayerTag.h"

void UKRGA_ExitCombat::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	AKRBaseCharacter* Character = Cast<AKRBaseCharacter>(ActorInfo->AvatarActor.Get());
	AKRPlayerController* PC = Cast<AKRPlayerController>(ActorInfo->PlayerController.Get());
	UKREquipmentManagerComponent* EquipComp = Character ? Character->FindComponentByClass<UKREquipmentManagerComponent>() : nullptr;
	UKRAbilitySystemComponent* KRASC = GetKRAbilitySystemComponentFromActorInfo();

	if (!Character || !PC || !EquipComp || !KRASC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	TArray<UKREquipmentInstance*> AllWeapons = EquipComp->GetEquipmentInstancesOfType(UKRWeaponInstance::StaticClass());
	bool bBroadcastDone = false;
	for (UKREquipmentInstance* Equip : AllWeapons)
	{
		if (UKRWeaponInstance* Weapon = Cast<UKRWeaponInstance>(Equip))
		{
			const bool bBroadcast = !bBroadcastDone;
			Weapon->DeactivateWeapon(Character, PC, KRASC, EWeaponMessageAction::Unequipped, !bBroadcastDone);
			if (bBroadcast)
			{
				bBroadcastDone = true;
			}
		}
	}

	Character->GetMesh()->LinkAnimClassLayers(nullptr);
	KRASC->RemoveLooseGameplayTag(KRTAG_PLAYER_MODE_SWORD);
	KRASC->RemoveLooseGameplayTag(KRTAG_PLAYER_MODE_GUN);
	KRASC->AddLooseGameplayTag(KRTAG_PLAYER_MODE_BASE);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
