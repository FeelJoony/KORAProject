#include "KRFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "Interface/PawnCombatInterface.h"

void UKRFunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
	UKRAbilitySystemComponent* ASC = NativeGetKRASCFromActor(InActor);

	if (!ASC->HasMatchingGameplayTag(TagToAdd))
	{
		ASC->AddLooseGameplayTag(TagToAdd);
	}
}

void UKRFunctionLibrary::RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
{
	UKRAbilitySystemComponent* ASC = NativeGetKRASCFromActor(InActor);

	if (ASC->HasMatchingGameplayTag(TagToRemove))
	{
		ASC->RemoveLooseGameplayTag(TagToRemove);
	}
}

void UKRFunctionLibrary::BP_DoseActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, EKRConfirmType& OutConfirmType)
{
	OutConfirmType = NativeDoesActorHaveTag(InActor, TagToCheck) ? EKRConfirmType::Yes : EKRConfirmType::No;
}

UPawnCombatComponent* UKRFunctionLibrary::BP_GetPawnCombatComponentFromActor(AActor* InActor,
	EKRValidType& OutValidType)
{
	UPawnCombatComponent* CombatComponent = NativeGetPawnCombatComponentFromActor(InActor);
	OutValidType = CombatComponent ? EKRValidType::Valid : EKRValidType::InValid;

	return CombatComponent;
}

UKRAbilitySystemComponent* UKRFunctionLibrary::NativeGetKRASCFromActor(AActor* InActor)
{
	check(InActor);

	return CastChecked<UKRAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
}

UPawnCombatComponent* UKRFunctionLibrary::NativeGetPawnCombatComponentFromActor(AActor* InActor)
{
	check(InActor);

	if (IPawnCombatInterface* PawnCombatInterface = Cast<IPawnCombatInterface>(InActor))
	{
		return PawnCombatInterface->GetPawnCombatComponent();
	}

	return nullptr;
}

bool UKRFunctionLibrary::NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck)
{
	UKRAbilitySystemComponent* ASC = NativeGetKRASCFromActor(InActor);

	return ASC->HasMatchingGameplayTag(TagToCheck);
}
