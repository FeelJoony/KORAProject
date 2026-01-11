// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/KRAnimNotify_LaunchCharacter.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UKRAnimNotify_LaunchCharacter::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                           const FAnimNotifyEventReference& EventReference)
{
  	Super::Notify(MeshComp, Animation, EventReference);
	if (!MeshComp) return;

	if (ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
	{
		FRotator CharacterRotation = Character->GetActorRotation();

		FVector FinalVelocity = CharacterRotation.RotateVector(LaunchDirection);
		
		Character->LaunchCharacter(FinalVelocity,XYOverride, ZOverride);
	}
}
