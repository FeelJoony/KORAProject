#include "GAS/Abilities/EnemyAbility/KRGA_BossFight.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AI/KREnemyPawn.h"
#include "Components/StateTreeAIComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayTag/KREventTag.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"

UKRGA_BossFight::UKRGA_BossFight(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UKRGA_BossFight::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ShowBossWidget();

	if (bEndOnDeath)
	{
		DeathEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			KRTAG_EVENT_COMBAT_DEATH,
			nullptr,
			true,
			true
		);
		if (DeathEventTask)
		{
			DeathEventTask->EventReceived.AddDynamic(this, &ThisClass::OnDeathEventReceived);
			DeathEventTask->ReadyForActivation();
		}
	}
}

void UKRGA_BossFight::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (bWasCancelled)
	{
		HideBossWidget();
	}

	if (EndAbilityTags.Num() > 0)
	{
		SendStateTreeEvent(EndAbilityTags);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_BossFight::ShowBossWidget()
{
	AKREnemyPawn* Enemy = Cast<AKREnemyPawn>(GetAvatarActorFromActorInfo());
	UKRAbilitySystemComponent* EnemyASC = Cast<UKRAbilitySystemComponent>(Enemy->GetAbilitySystemComponent());
	if (!EnemyASC) return;

	UWorld* World = GetWorld();
	if (!World) return;

	FKRUIMessage_Boss BossMessage;
	BossMessage.BossASC = EnemyASC;
	BossMessage.BossNameKey = BossNameKey;

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(World);
	MessageSubsystem.BroadcastMessage(FKRUIMessageTags::Boss(), BossMessage);
}

void UKRGA_BossFight::HideBossWidget()
{
	UWorld* World = GetWorld();
	if (!World || World->bIsTearingDown) return;

	FKRUIMessage_Boss BossMessage;
	BossMessage.BossASC = nullptr;
	BossMessage.BossNameKey = NAME_None;

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(World);
	MessageSubsystem.BroadcastMessage(FKRUIMessageTags::Boss(), BossMessage);
}

void UKRGA_BossFight::SendStateTreeEvent(const FGameplayTagContainer& Tags)
{
	if (const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo())
	{
		if (ActorInfo->OwnerActor.IsValid())
		{
			if (AActor* OwnerActor = ActorInfo->OwnerActor.Get())
			{
				if (UStateTreeAIComponent* StateTreeAIComp = OwnerActor->GetComponentByClass<UStateTreeAIComponent>())
				{
					for (const FGameplayTag& Tag : Tags)
					{
						StateTreeAIComp->SendStateTreeEvent(Tag);
					}
				}
			}
		}
	}
}

void UKRGA_BossFight::OnDeathEventReceived(FGameplayEventData Payload)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
