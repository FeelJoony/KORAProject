#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
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
	FText Text;

	UPROPERTY(BlueprintReadWrite)
	FText SubText;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> InteractionAbilityToGrant;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> TargetASC = nullptr;

	UPROPERTY(BlueprintReadOnly)
	FGameplayAbilitySpecHandle TargetInteractionAbilityHandle;
	
	UPROPERTY(BlueprintReadWrite)
	TSoftClassPtr<UUserWidget> InteractionWidgetClass;
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