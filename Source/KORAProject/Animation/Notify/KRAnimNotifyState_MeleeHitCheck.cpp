#include "Animation/Notify/KRAnimNotifyState_MeleeHitCheck.h"
#include "GAS/Abilities/HeroAbilities/Attack/KRGameplayAbility_MeleeAttack.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemBlueprintLibrary.h"

UKRAnimNotifyState_MeleeHitCheck::UKRAnimNotifyState_MeleeHitCheck()
{
}

FString UKRAnimNotifyState_MeleeHitCheck::GetNotifyName_Implementation() const
{
	return TEXT("Melee Hit Check");
}

void UKRAnimNotifyState_MeleeHitCheck::NotifyBegin(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp)
	{
		return;
	}

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!IsValid(OwnerActor))
	{
		return;
	}

	if (UKRGameplayAbility_MeleeAttack* MeleeAbility = GetActiveMeleeAttackAbility(OwnerActor))
	{
		MeleeAbility->BeginHitCheck();
	}
}

void UKRAnimNotifyState_MeleeHitCheck::NotifyTick(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!MeshComp)
	{
		return;
	}

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!IsValid(OwnerActor))
	{
		return;
	}

	if (UKRGameplayAbility_MeleeAttack* MeleeAbility = GetActiveMeleeAttackAbility(OwnerActor))
	{
		MeleeAbility->PerformHitCheck();
	}
}

void UKRAnimNotifyState_MeleeHitCheck::NotifyEnd(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!IsValid(OwnerActor))
	{
		return;
	}

	if (UKRGameplayAbility_MeleeAttack* MeleeAbility = GetActiveMeleeAttackAbility(OwnerActor))
	{
		MeleeAbility->EndHitCheck();
	}
}

UKRGameplayAbility_MeleeAttack* UKRAnimNotifyState_MeleeHitCheck::GetActiveMeleeAttackAbility(AActor* OwnerActor) const
{
	if (!OwnerActor)
	{
		return nullptr;
	}

	// ASC 조회
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor);
	if (!ASC)
	{
		return nullptr;
	}

	// 활성화된 MeleeAttack GA 찾기
	for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		// Ability가 활성화되어 있는지 확인
		if (!Spec.IsActive())
		{
			continue;
		}

		// MeleeAttack 타입인지 확인
		if (UKRGameplayAbility_MeleeAttack* MeleeAbility = Cast<UKRGameplayAbility_MeleeAttack>(Spec.GetPrimaryInstance()))
		{
			return MeleeAbility;
		}
	}

	return nullptr;
}
