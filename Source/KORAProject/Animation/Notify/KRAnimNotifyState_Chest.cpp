#include "Animation/Notify/KRAnimNotifyState_Chest.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "Components/KRCharacterMovementComponent.h"
#include "GameplayTag/KRStateTag.h"

void UKRAnimNotifyState_Chest::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp) return;

	if (ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
	{
		if (UKRCharacterMovementComponent* KRCMC = Cast<UKRCharacterMovementComponent>(Character->GetCharacterMovement()))
		{
			KRCMC->StopMovementImmediately();
		}
		
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Character))
		{
			ASC->AddLooseGameplayTag(KRTAG_STATE_RESTRICT_INPUT);
		}
	}
}

void UKRAnimNotifyState_Chest::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	if (ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Character))
		{
			ASC->RemoveLooseGameplayTag(KRTAG_STATE_RESTRICT_INPUT);
		}
	}
}