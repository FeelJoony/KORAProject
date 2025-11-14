#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "InteractionOption.generated.h"

class IInteractableTarget;
class UUserWidget;

USTRUCT(BlueprintType)
struct FInteractionOption
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	TScriptInterface<IInteractableTarget> InteractableTarget;

	UPROPERTY(BlueprintReadWrite)
	int32 Priority = 3;

	UPROPERTY(BlueprintReadWrite)
	FText Text;

	UPROPERTY(BlueprintReadWrite)
	FText SubText;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> InteractionAbilityToGrant = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> TargetASC = nullptr;

	UPROPERTY(BlueprintReadOnly)
	FGameplayAbilitySpecHandle TargetInteractionAbilityHandle;
	
	UPROPERTY(BlueprintReadWrite)
	TSoftClassPtr<UUserWidget> InteractionWidgetClass = nullptr;

public:
	FORCEINLINE bool operator==(const FInteractionOption& Other) const
	{
		return InteractableTarget == Other.InteractableTarget &&
			Priority == Other.Priority &&
			Text.IdenticalTo(Other.Text) &&
			SubText.IdenticalTo(Other.SubText) &&
			InteractionAbilityToGrant == Other.InteractionAbilityToGrant &&
			TargetASC == Other.TargetASC &&
			TargetInteractionAbilityHandle == Other.TargetInteractionAbilityHandle &&
			InteractionWidgetClass == Other.InteractionWidgetClass;
	}
	
	FORCEINLINE bool operator!=(const FInteractionOption& Other) const
	{
		return !operator==(Other);
	}
};



USTRUCT(BlueprintType)
struct FInteractionQuery
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> RequestingActor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AController> RequestingController = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UObject> OptionalObjectData = nullptr;
};