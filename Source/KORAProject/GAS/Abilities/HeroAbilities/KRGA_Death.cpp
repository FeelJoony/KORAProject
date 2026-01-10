#include "GAS/Abilities/HeroAbilities/KRGA_Death.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/KRCurrencyComponent.h"
#include "GameplayTag/KRAbilityTag.h"
#include "GameplayTag/KREventTag.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "GameModes/KRBaseGameMode.h"

UKRGA_Death::UKRGA_Death(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FGameplayTagContainer Tags;
	Tags.AddTag(KRTAG_ABILITY_DEATH);
	SetAssetTags(Tags);

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	// Event.Combat.Death 이벤트 발생 시 자동 활성화
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = KRTAG_EVENT_COMBAT_DEATH;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UKRGA_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true ,true);
		return;
	}

	if (DeathEffectClass)
	{
		FGameplayEffectSpecHandle DeathSpec = MakeOutgoingGameplayEffectSpec(DeathEffectClass, GetAbilityLevel());
		if (DeathSpec.IsValid())
		{
			(void)ApplyGameplayEffectSpecToOwner(
				Handle,
				ActorInfo,
				ActivationInfo,
				DeathSpec);
		}
	}

	if (UKRCurrencyComponent* CurrencyComp = GetAvatarActorFromActorInfo()->FindComponentByClass<UKRCurrencyComponent>())
	{
		CurrencyComp->HandleDeath();
	}

	// UI Message Broadcast (Client Only)
	if (IsLocallyControlled())
	{
		FKRUIMessage_Info DeathMessage;
		DeathMessage.Context = EInfoContext::Death;
		DeathMessage.StringTableKey = TEXT("Info_Death");

		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
		MessageSystem.BroadcastMessage(FKRUIMessageTags::SaveDeathLevelInfo(), DeathMessage);
	}

	// Respawn Timer (Server Only) - GameMode에 위임하여 어빌리티 수명과 무관하게 동작
	if (GetOwningActorFromActorInfo()->HasAuthority())
	{
		if (AKRBaseGameMode* GM = Cast<AKRBaseGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->StartRespawnSequence(Cast<AController>(CurrentActorInfo->PlayerController), 3.2f);
		}
	}

	if (DeathMontage)
	{
		UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			DeathMontage,
			1.0f,
			NAME_None,
			false
			);

		if (Task)
		{
			Task->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
			Task->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
			Task->OnCancelled.AddDynamic(this, &ThisClass::OnMontageInterrupted);
			Task->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageCompleted);
			Task->ReadyForActivation();
		}
	}
	else
	{
		EndAbility(Handle,ActorInfo,ActivationInfo,true,false);
	}
}

void UKRGA_Death::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                             const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (bStartRagdollOnEnd)
	{
		if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
		{
			Character->GetMesh()->SetSimulatePhysics(true);
			Character->GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
			Character->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_Death::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UKRGA_Death::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
