#include "KRGA_Interact.h"

#include "AbilitySystemComponent.h"
#include "SubSystem/KRUIRouterSubsystem.h"

bool UKRGA_Interact::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if (ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		for (const FGameplayTag& Tag : GameplayAbilityTags)
		{
			if (ASC->HasMatchingGameplayTag(Tag))
			{
				return true;
			}
		}
	}
	return false;
}

void UKRGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	bInputProcessed = false;
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ProcessInteraction(Handle, ActorInfo, ActivationInfo);
}

void UKRGA_Interact::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_Interact::ProcessInteraction(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (bInputProcessed)
	{
		UE_LOG(LogTemp, Warning, TEXT("[KRGA_Interact] ProcessInteraction - Already processed, skipping"));
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	for (FGameplayTag& Tag : GameplayAbilityTags)
	{
		if (ASC->HasMatchingGameplayTag(Tag))
		{
			FGameplayTagContainer TagContainer(Tag);
			bool bActivated = ASC->TryActivateAbilitiesByTag(TagContainer);
			if (bActivated)
			{
				bInputProcessed = true;
				HideInteractionUI();
				EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
				return;
			}
		}
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

void UKRGA_Interact::HideInteractionUI()
{
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GI = World->GetGameInstance())
		{
			if (UKRUIRouterSubsystem* Router = GI->GetSubsystem<UKRUIRouterSubsystem>())
			{
				if (Router->IsRouteOpen(InteractionUIRouteName))
				{
					Router->CloseRoute(InteractionUIRouteName);
				}
			}
		}
	}
}
