#include "GAS/Abilities/EnemyAbility/KRGA_Enemy_Die.h"
#include "AI/KREnemyPawn.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "GameplayTag/KREnemyTag.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/GameplayMessageSubsystem.h"

void UKRGA_Enemy_Die::ActivateAbility(const FGameplayAbilitySpecHandle Handle, 
										const FGameplayAbilityActorInfo* ActorInfo, 
										const FGameplayAbilityActivationInfo ActivationInfo, 
										const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (DieMontage)
	{
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			DieMontage,
			1.f,
			NAME_None
		);

		OrganizeStateTag();
		ActivationDie();

		UGameplayMessageSubsystem& Subsystem = UGameplayMessageSubsystem::Get(GetWorld());

		FKillMonsterMessage Message;
		Message.MonsterIndex = 1;
		Subsystem.BroadcastMessage(KRTAG_ENEMY_AISTATE_DEAD, Message);
	}
}

void UKRGA_Enemy_Die::EndAbility(const FGameplayAbilitySpecHandle Handle, 
									const FGameplayAbilityActorInfo* ActorInfo, 
									const FGameplayAbilityActivationInfo ActivationInfo, 
									bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	AKREnemyPawn* Enemy = Cast<AKREnemyPawn>(GetAvatarActorFromActorInfo());
	UKRAbilitySystemComponent* EnemyASC = Cast<UKRAbilitySystemComponent>(Enemy->GetAbilitySystemComponent());
	if (!EnemyASC) return;

	if (EnemyASC->HasMatchingGameplayTag(KRTAG_ENEMY_AISTATE_DEAD))
	{
		EnemyASC->RemoveLooseGameplayTag(KRTAG_ENEMY_AISTATE_DEAD);
	}
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DestroyTimerHandle);
	}
}

void UKRGA_Enemy_Die::OrganizeStateTag()
{
	AKREnemyPawn* Enemy = Cast<AKREnemyPawn>(GetAvatarActorFromActorInfo());
	UKRAbilitySystemComponent* EnemyASC = Cast<UKRAbilitySystemComponent>(Enemy->GetAbilitySystemComponent());
	if (!EnemyASC) return;

	const FGameplayTagContainer& CurrentTags = EnemyASC->GetOwnedGameplayTags();

	FGameplayTagContainer TagsToKeep;
	TagsToKeep.AddTag(KRTAG_ENEMY_IMMUNE_GRAPPLE);
	TagsToKeep.AddTag(KRTAG_ENEMY_AISTATE_DEAD);

	FGameplayTagContainer TagsToRemove = CurrentTags;
	TagsToRemove.RemoveTags(TagsToKeep);

	for (const FGameplayTag& Tag : TagsToRemove)
	{
		if (EnemyASC->HasMatchingGameplayTag(Tag))
		{
			EnemyASC->RemoveLooseGameplayTag(Tag);
		}
	}
}

void UKRGA_Enemy_Die::ActivationDie()
{
	MontageTask->OnCompleted.AddDynamic(this, &UKRGA_Enemy_Die::OnMontageEnded);
	MontageTask->OnCancelled.AddDynamic(this, &UKRGA_Enemy_Die::OnMontageEnded);
	MontageTask->OnInterrupted.AddDynamic(this, &UKRGA_Enemy_Die::OnMontageEnded);
	MontageTask->OnBlendOut.AddDynamic(this, &UKRGA_Enemy_Die::OnMontageEnded);
	MontageTask->ReadyForActivation();
}

void UKRGA_Enemy_Die::OnMontageEnded()
{
	AKREnemyPawn* Enemy = Cast<AKREnemyPawn>(GetAvatarActorFromActorInfo());
	USkeletalMeshComponent* Mesh = Enemy->GetMesh();
	if (Mesh)
	{
		Mesh->bPauseAnims = true;
		Mesh->bNoSkeletonUpdate = true;
	}

	AAIController* AIC = Cast<AAIController>(Enemy->GetController());
	if (IsValid(AIC))
	{
		AIC->BrainComponent->StopLogic(TEXT("Dead"));
		AIC->UnPossess();
	}

	if (UWorld* World = GetWorld())
	{
		TWeakObjectPtr<AActor> WeakEnemy = Enemy;

		World->GetTimerManager().SetTimer(
			DestroyTimerHandle,
			[WeakEnemy]()
			{
				if (WeakEnemy.IsValid())
				{
					WeakEnemy->Destroy();
				}
			},
			DestroyDelay,
			false
		);
	}
}

void UKRGA_Enemy_Die::ExternalAbilityEnded()
{
}