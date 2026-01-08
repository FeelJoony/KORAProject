// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/EnemyAbility/KRGA_EnemyMontage.h"

#include "Components/StateTreeAIComponent.h"

UKRGA_EnemyMontage::UKRGA_EnemyMontage(const FObjectInitializer& ObjectInitializer)
{
	
}

void UKRGA_EnemyMontage::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (ActorInfo->OwnerActor.IsValid())
	{
		if (AActor* OwnerActor = ActorInfo->OwnerActor.Get())
		{
			if (UStateTreeAIComponent* StateTreeAIComp = OwnerActor->GetComponentByClass<UStateTreeAIComponent>())
			{
				if (AbilityTags.Num() <= 0)
				{
					UE_LOG(LogTemp, Warning, TEXT("Montage AbilityTags is Empty"));
				}

				StateTreeAIComp->SendStateTreeEvent(AbilityTags.GetByIndex(0));
			}
		}
	}
}

void UKRGA_EnemyMontage::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActorInfo->OwnerActor.IsValid())
	{
		if (AActor* OwnerActor = ActorInfo->OwnerActor.Get())
		{
			if (UStateTreeAIComponent* StateTreeAIComp = OwnerActor->GetComponentByClass<UStateTreeAIComponent>())
			{
				if (EndAbilityTags.Num() <= 0)
				{
					UE_LOG(LogTemp, Warning, TEXT("Montage AbilityTags is Empty"));
				}

				StateTreeAIComp->SendStateTreeEvent(EndAbilityTags.GetByIndex(0));
			}
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
