#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KORATypes/KREnumTypes.h"
#include "KRFunctionLibrary.generated.h"

struct FGameplayTag;
class UPawnCombatComponent;
class UKRAbilitySystemComponent;

UCLASS()
class KORAPROJECT_API UKRFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "KR|FunctionLibrary")
	static void AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd);

	UFUNCTION(BlueprintCallable, Category = "KR|FunctionLibrary")
	static void RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove);

	UFUNCTION(BlueprintCallable, Category = "KR|FunctionLibrary", meta = (DisplayName = "Does Actor Have Tag", ExpandEnumAsExecs = "OutConfirmType"))
	static void BP_DoseActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, EKRConfirmType& OutConfirmType);

	UFUNCTION(BlueprintCallable, Category = "KR|FunctionLibrary", meta = (DisplayName = "Get Pawn Combat Component From Actor", ExpandEnumAsExecs = "OutValidType"))
	static UPawnCombatComponent* BP_GetPawnCombatComponentFromActor(AActor* InActor, EKRValidType& OutValidType);
	
	static UKRAbilitySystemComponent* NativeGetKRASCFromActor(AActor* InActor);
	static UPawnCombatComponent* NativeGetPawnCombatComponentFromActor(AActor* InActor);
	static bool NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck);
	
	
	
};
