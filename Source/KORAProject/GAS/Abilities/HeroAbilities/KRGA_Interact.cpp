#include "KRGA_Interact.h"

#include "AbilitySystemComponent.h"
#include "GAS/Tasks/AbilityTask_WaitForInteractableTargets_SingleLineTrace.h"
#include "GAS/Tasks/AbilityTask_GrantNearbyInteraction.h"
#include "GAS/KRGameplayTags.h"
#include "Controllers/KRHeroController.h"
#include "Interaction/InteractionStatics.h"
#include "Interaction/InteractableTarget.h"
#include "UI/Indicators/IndicatorDescriptor.h"
#include "UI/Indicators/KRIndicatorManagerComponent.h"

UKRGA_Interact::UKRGA_Interact()
{
	InteractionRange = 500.f;
	InteractionScanRate = 0.1f;
	bShowLineTraceDebug = false;
	bShowSphereTraceDebug = false;
}

void UKRGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	if (!AvatarActor)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilityTask_WaitForInteractableTargets_SingleLineTrace* WaitTask =
		UAbilityTask_WaitForInteractableTargets_SingleLineTrace::WaitForInteractableTargets_SingleLineTrace(
			this,
			InteractionRange,
			InteractionScanRate,
			bShowLineTraceDebug
		);
	WaitTask->ReadyForActivation();

	UAbilityTask_GrantNearbyInteraction* GrantTask =
		UAbilityTask_GrantNearbyInteraction::GrantAbilitiesForNearbyInteractors(
			this,
			bShowSphereTraceDebug,
			InteractionRange,
			InteractionScanRate
		);
	GrantTask->ReadyForActivation();
}

void UKRGA_Interact::UpdateInteractions(const TArray<FInteractionOption>& InteractiveOptions)
{
	if (AKRHeroController* PC = Cast<AKRHeroController>(CurrentActorInfo->PlayerController.Get()))
		{
		if (UKRIndicatorManagerComponent* IndicatorManager = PC->FindComponentByClass<UKRIndicatorManagerComponent>())
		{
			for (UIndicatorDescriptor* Indicator : Indicators)
			{
				IndicatorManager->RemoveIndicator(Indicator);
			}
			Indicators.Reset();
			for (const FInteractionOption& InteractionOption : InteractiveOptions)
			{
				AActor* InteractableTargetActor = UInteractionStatics::GetActorFromInteractableTarget(InteractionOption.InteractableTarget);
				TSoftClassPtr<UUserWidget> InteractionWidgetClass = InteractionOption.InteractionWidgetClass.IsNull() ? DefaultInteractionWidgetClass : InteractionOption.InteractionWidgetClass;
				UIndicatorDescriptor* Indicator = NewObject<UIndicatorDescriptor>();
				Indicator->SetDataObject(InteractableTargetActor);
				Indicator->SetSceneComponent(InteractableTargetActor->GetRootComponent());
				Indicator->SetIndicatorClass(InteractionWidgetClass);
				IndicatorManager->AddIndicator(Indicator);
				Indicators.Add(Indicator);
			}
		}
		else
			{
				//TODO This should probably be a noisy warning. Why are we updating interactions on a PC that can never do anything with them?
			}
		}
	CurrentOptions = InteractiveOptions;
	//UE_LOG(LogTemp, Warning, TEXT("[Update] CurrentOptions updated: %d options"), CurrentOptions.Num());
}

void UKRGA_Interact::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	if (CurrentOptions.Num() == 0)
	{
		return;
	}
	//if (UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo())
	{
		const FInteractionOption& InteractionOption = CurrentOptions[0];
		
		AActor* Instigator = GetAvatarActorFromActorInfo();
		AActor* InteractableTargetActor = UInteractionStatics::GetActorFromInteractableTarget(InteractionOption.InteractableTarget);

		FGameplayEventData Payload;
		Payload.EventTag = KRTag_State_Acting_Interacting;
		Payload.Instigator = Instigator;
		Payload.Target = InteractableTargetActor;

		InteractionOption.InteractableTarget->CustomizeInteractionEventData(KRTag_State_Acting_Interacting, Payload);

		AActor* TargetActor = const_cast<AActor*>(ToRawPtr(Payload.Target));// GA 실행 주체
		
		FGameplayAbilityActorInfo ActorInfomation;
		ActorInfomation.InitFromActor(InteractableTargetActor, TargetActor, InteractionOption.TargetASC);

		const bool bSuccess = InteractionOption.TargetASC->TriggerAbilityFromGameplayEvent( InteractionOption.TargetInteractionAbilityHandle, &ActorInfomation, KRTag_State_Acting_Interacting, &Payload, *InteractionOption.TargetASC );
	}
}
