#include "AbilityTask_WaitForInteractableTargets_SingleLineTrace.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Interaction/InteractableTarget.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"

UAbilityTask_WaitForInteractableTargets_SingleLineTrace* UAbilityTask_WaitForInteractableTargets_SingleLineTrace::
WaitForInteractableTargets_SingleLineTrace(UGameplayAbility* OwningAbility,	float InteractionScanRange,
	float InteractionScanRate, bool bShowDebug)
{
	UAbilityTask_WaitForInteractableTargets_SingleLineTrace* MyObj = NewAbilityTask<UAbilityTask_WaitForInteractableTargets_SingleLineTrace>(OwningAbility);
	MyObj->LineTraceRange = InteractionScanRange;
	MyObj->LineTraceScanRate = InteractionScanRate;
	MyObj->bShowDebug= bShowDebug;
	return MyObj;
}

void UAbilityTask_WaitForInteractableTargets_SingleLineTrace::Activate()
{
	Super::Activate();
	SetWaitingOnAvatar();//바로 종료 X

	if(UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(TimerHandle,this,&ThisClass::PerformTrace,LineTraceScanRate,true);
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("[AbilityTask_SingleLineTrace] Activate FAILED"));
	}
}

void UAbilityTask_WaitForInteractableTargets_SingleLineTrace::OnDestroy(bool AbilityEnded)
{
	if (UWorld* World =  GetWorld())
	{
		World->GetTimerManager().ClearTimer(TimerHandle);
	}

	Super::OnDestroy(AbilityEnded);
}

void UAbilityTask_WaitForInteractableTargets_SingleLineTrace::PerformTrace()
{
	AActor* AvatarActor = Ability->GetCurrentActorInfo()->AvatarActor.Get(); //Task 방식
	UWorld* World = GetWorld();
	if(!AvatarActor || !World)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AbilityTask_SingleLineTrace] Perform Trace FAILED"));
	}

	FVector TraceStart = AvatarActor->GetActorLocation(); //카메라 중심으로 Fix됨
	FVector TraceEnd;
	FixTraceDirection(TraceStart, LineTraceRange,TraceEnd);
	
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(AvatarActor);
	GetWorld()->LineTraceSingleByProfile(HitResult, TraceStart, TraceEnd, FName("Interaction_Line"), Params);// !!!
	
	TScriptInterface<IInteractableTarget> InteractableTarget;

	TScriptInterface<IInteractableTarget> InteractableActor(HitResult.GetActor());
	if (InteractableActor)
	{
		InteractableTarget=InteractableActor;
	}
	TScriptInterface<IInteractableTarget> InteractableComponent(HitResult.GetComponent());
	if (InteractableComponent)
	{
		InteractableTarget=InteractableComponent;
	}

	UpdateInteractableOptions(InteractionQuery, InteractableTarget);

	// Debug test
	if (bShowDebug)
	{
		FColor DebugColor = HitResult.bBlockingHit ? FColor::Red : FColor::Green;
		if (HitResult.bBlockingHit)
		{
			DrawDebugLine(World, TraceStart, HitResult.Location, DebugColor, false, LineTraceScanRate);
			DrawDebugSphere(World, HitResult.Location, 5, 16, DebugColor, false, LineTraceScanRate);
		}
		else
		{
			DrawDebugLine(World, TraceStart, TraceEnd, DebugColor, false, LineTraceScanRate);
		}
	}
}

void UAbilityTask_WaitForInteractableTargets_SingleLineTrace::FixTraceDirection(
	FVector& TraceStart, float MaxRange, FVector& OutTraceEnd) const
{
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	if (!ensure(PC))
	{
		return;
	}
	
	FRotator ViewRot;
	PC->GetPlayerViewPoint(TraceStart, ViewRot);

	const FVector ViewDir = ViewRot.Vector();
	OutTraceEnd = TraceStart*(ViewDir*MaxRange);
	
	//카메라가 벽을 통과하는 일이 발생할 시 Lyra의 clip 로직 구현 필요

	// Lyra Aim 보정 로직 생략
}

void UAbilityTask_WaitForInteractableTargets_SingleLineTrace::UpdateInteractableOptions(
	const FInteractionQuery& InInteractionQuery, const TScriptInterface<IInteractableTarget>& InteractableTarget)
{
	// Graple 전용 로직으로 변경 예정
	// TArray<FInteractionOption> NewOptions;
	// TArray<FInteractionOption> TempOptions;
	// FInteractionOptionBuilder InteractionBuilder(InteractableTarget, TempOptions);
	// InteractableTarget->GatherInteractionOptions(InInteractionQuery, InteractionBuilder);
	//
	// FGameplayAbilitySpec* InteractionAbilitySpec = nullptr;
	// for (FInteractionOption& Option : TempOptions)
	// {
	// 	if (Option.TargetASC && Option.TargetInteractionAbilityHandle.IsValid())
	// 	{
	// 		InteractionAbilitySpec = Option.TargetASC->FindAbilitySpecFromHandle(Option.TargetInteractionAbilityHandle);
	// 	}
	// 	else if (Option.InteractionAbilityToGrant)
	// 	{
	// 		InteractionAbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(Option.InteractionAbilityToGrant);
	//
	// 		if (InteractionAbilitySpec)
	// 		{
	// 			Option.TargetASC = AbilitySystemComponent.Get();
	// 			Option.TargetInteractionAbilityHandle = InteractionAbilitySpec->Handle;
	// 		}
	// 	}
	//
	// 	if (InteractionAbilitySpec)
	// 	{
	// 		if (InteractionAbilitySpec->Ability->CanActivateAbility(InteractionAbilitySpec->Handle, AbilitySystemComponent->AbilityActorInfo.Get()))
	// 		{
	// 			NewOptions.Add(Option);
	// 		}
	// 	}
	// }
}
