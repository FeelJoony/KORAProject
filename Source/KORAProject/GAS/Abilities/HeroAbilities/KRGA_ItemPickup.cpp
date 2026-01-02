#include "KRGA_ItemPickup.h"
#include "AbilitySystemComponent.h"
#include "Interaction/KRQuestItemPickup.h"
#include "Player/KRPlayerController.h"
#include "GameplayTag/KRAbilityTag.h"

UKRGA_ItemPickup::UKRGA_ItemPickup(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FGameplayTagContainer Tags;
	Tags.AddTag(KRTAG_ABILITY_ITEMPICKUP);
	SetAssetTags(Tags);
}

void UKRGA_ItemPickup::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AKRQuestItemPickup* ItemPickup = GetItemPickupFromPlayerController(ActorInfo);
	if (!ItemPickup)
	{
		UE_LOG(LogTemp, Warning, TEXT("[KRGA_ItemPickup] No ItemPickup found from PlayerController"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ItemPickup->PickupItem();
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

void UKRGA_ItemPickup::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

AKRQuestItemPickup* UKRGA_ItemPickup::GetItemPickupFromPlayerController(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return nullptr;
	}

	APawn* AvatarPawn = Cast<APawn>(ActorInfo->AvatarActor.Get());
	if (!AvatarPawn)
	{
		return nullptr;
	}

	AKRPlayerController* PC = Cast<AKRPlayerController>(AvatarPawn->GetController());
	if (!PC)
	{
		return nullptr;
	}

	AInteractableActorBase* InteractableActor = PC->GetCurrentInteractableActor();
	return Cast<AKRQuestItemPickup>(InteractableActor);
}
