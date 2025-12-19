#include "GAS/Abilities/HeroAbilities/KRGA_EnterCombat.h"
#include "Equipment/KREquipmentManagerComponent.h"
#include "Components/KRCombatComponent.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "Characters/KRBaseCharacter.h"
#include "GameplayTag/KRPlayerTag.h"
#include "GameplayTag/KRItemTypeTag.h"
#include "Item/Weapons/KRWeaponBase.h"
#include "Item/Weapons/KRMeleeWeapon.h"
#include "Item/Weapons/KRRangeWeapon.h"

UKRGA_EnterCombat::UKRGA_EnterCombat(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// 이미 Gun 모드면 발동 차단
	ActivationBlockedTags.AddTag(KRTAG_PLAYER_MODE_GUN);
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

	// 전투 모드 활성화 (GA 부여, IMC 추가, AnimLayer 변경, 무기 가시성)
	if (!EquipComp->ActivateCombatMode(KRTAG_ITEMTYPE_EQUIP_GUN))
	{
		UE_LOG(LogTemp, Warning, TEXT("EnterCombat: Failed to activate combat mode for Gun!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 태그 변경
	KRASC->RemoveLooseGameplayTag(KRTAG_PLAYER_MODE_BASE);
	KRASC->RemoveLooseGameplayTag(KRTAG_PLAYER_MODE_SWORD);
	KRASC->AddLooseGameplayTag(KRTAG_PLAYER_MODE_GUN);

	UE_LOG(LogTemp, Log, TEXT("EnterCombat: Successfully entered Gun combat mode"));
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UKRGA_EnterCombat::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}