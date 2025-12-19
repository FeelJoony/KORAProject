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

    // 1. 현재 활성화된 모드 확인
    FGameplayTag CurrentModeTag = FGameplayTag::EmptyTag;
    FGameplayTag NewModeTag = FGameplayTag::EmptyTag;
    FGameplayTag NewWeaponType = FGameplayTag::EmptyTag;

    if (KRASC->HasMatchingGameplayTag(KRTAG_PLAYER_MODE_GUN))
    {
        // 현재 총 -> 검으로 변경
        CurrentModeTag = KRTAG_PLAYER_MODE_GUN;
        NewModeTag = KRTAG_PLAYER_MODE_SWORD;
        NewWeaponType = KRTAG_ITEMTYPE_EQUIP_SWORD;
    }
    else if (KRASC->HasMatchingGameplayTag(KRTAG_PLAYER_MODE_SWORD))
    {
        // 현재 검 -> 총으로 변경
        CurrentModeTag = KRTAG_PLAYER_MODE_SWORD;
        NewModeTag = KRTAG_PLAYER_MODE_GUN;
        NewWeaponType = KRTAG_ITEMTYPE_EQUIP_GUN;
    }
    else
    {
        // 전투 모드가 아님 -> 스왑 불가 (혹은 기본 무기 장착)
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // 2. 장비 매니저를 통해 전투 모드 전환 (ActivateCombatMode 내부에서 Deactivate -> Activate 자동 수행)
    if (!EquipComp->ActivateCombatMode(NewWeaponType))
    {
        // 전환 실패 (해당 무기가 없거나 등)
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // 3. 태그 교체
    KRASC->RemoveLooseGameplayTag(CurrentModeTag);
    KRASC->AddLooseGameplayTag(NewModeTag);

    UE_LOG(LogTemp, Log, TEXT("WeaponSwap: Swapped from [%s] to [%s]"), *CurrentModeTag.ToString(), *NewModeTag.ToString());

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UKRGA_WeaponSwap::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
