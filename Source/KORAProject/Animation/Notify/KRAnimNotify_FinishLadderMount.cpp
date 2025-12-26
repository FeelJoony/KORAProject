#include "Animation/Notify/KRAnimNotify_FinishLadderMount.h"
#include "GameFramework/Character.h"
#include "Components/KRCharacterMovementComponent.h"

void UKRAnimNotify_FinishLadderMount::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;
	
	if (ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
	{
		if (UKRCharacterMovementComponent* KRCMC = Cast<UKRCharacterMovementComponent>(Character->GetCharacterMovement()))
		{
			KRCMC->SetLadderMounting(false);
			
			UE_LOG(LogTemp, Warning, TEXT("[Notify] Ladder Mount Finished. Movement Unlocked!"));
		}
	}
}