#include "Characters/KRPawnExtensionComponent.h"

const FName UKRPawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");

UKRPawnExtensionComponent::UKRPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void UKRPawnExtensionComponent::SetPawnData(const class UKRPawnData* NewPawnData)
{
	APawn* Pawn = GetPawnChecked<APawn>();
	if (Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		return;
	}

	PawnData = NewPawnData;
}
