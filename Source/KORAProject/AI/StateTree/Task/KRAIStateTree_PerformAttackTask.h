// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KRAIStateTreeTaskBase.h"
#include "KRAIStateTree_PerformAttackTask.generated.h"

UENUM(BlueprintType, Category = "Attack",
	meta = (DisplayName = "Attack Selection Mode",
		Description = "Determines how the attack ability is selected.",
		Tooltip = "Determines how the attack ability is selected"))
enum class EKRAI_AttackSelectMode : uint8
{
	Sequential	UMETA(DisplayName = "Sequential"),
	Random		UMETA(DisplayName = "Random")
};

USTRUCT(BlueprintType, Category = "Attack",
	meta = (DisplayName = "Attack Ability Info",
		Description = "Contains information about an attack ability, including its tag and weight for selection.",
		Tooltip = "Contains information about an attack ability, including its tag and weight for selection."))
struct KORAPROJECT_API FKRAIStateTree_PerformAttackAbilityInfo
{
	GENERATED_BODY()

	
};

UCLASS()
class KORAPROJECT_API UKRAIStateTree_PerformAttackTask : public UKRAIStateTreeTaskBase
{
	GENERATED_BODY()
};
