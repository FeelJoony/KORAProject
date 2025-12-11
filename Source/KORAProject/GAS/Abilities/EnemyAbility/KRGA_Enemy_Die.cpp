#include "GAS/Abilities/EnemyAbility/KRGA_Enemy_Die.h"
#include "Characters/KREnemyCharacter.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "GameplayTag/KREnemyTag.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

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

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor);
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	AKREnemyCharacter* Enemy = Cast<AKREnemyCharacter>(GetAvatarActorFromActorInfo());
	if (Enemy->DieMontage)
	{
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			Enemy->DieMontage,
			1.f,
			NAME_None
		);

		OrganizeStateTag();
		ActivationDie();
	}
}

void UKRGA_Enemy_Die::EndAbility(const FGameplayAbilitySpecHandle Handle, 
									const FGameplayAbilityActorInfo* ActorInfo, 
									const FGameplayAbilityActivationInfo ActivationInfo, 
									bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	AKREnemyCharacter* Enemy = Cast<AKREnemyCharacter>(GetAvatarActorFromActorInfo());
	UKRAbilitySystemComponent* EnemyASC = Cast<UKRAbilitySystemComponent>(Enemy->GetEnemyAbilitySystemCompoent());
	if (!EnemyASC) return;

	if (EnemyASC->HasMatchingGameplayTag(KRTAG_ENEMY_AISTATE_DEAD))
	{
		EnemyASC->RemoveLooseGameplayTag(KRTAG_ENEMY_AISTATE_DEAD);
	}
}

void UKRGA_Enemy_Die::OrganizeStateTag()
{
	AKREnemyCharacter* Enemy = Cast<AKREnemyCharacter>(GetAvatarActorFromActorInfo());
	UKRAbilitySystemComponent* EnemyASC = Cast<UKRAbilitySystemComponent>(Enemy->GetEnemyAbilitySystemCompoent());
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
	AKREnemyCharacter* Enemy = Cast<AKREnemyCharacter>(GetAvatarActorFromActorInfo());
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
}

void UKRGA_Enemy_Die::ExternalAbilityEnded()
{
}