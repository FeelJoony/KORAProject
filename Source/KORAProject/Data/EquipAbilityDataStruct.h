// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Interface/TableKey.h"
#include "EquipAbilityDataStruct.generated.h"

USTRUCT(BlueprintType)
struct FKREquipAbilityModifierRow
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag AbilityTypeTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseValue = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float IncValue = 0.f;
};

USTRUCT(BlueprintType)
struct KORAPROJECT_API FEquipAbilityDataStruct  : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()

	FEquipAbilityDataStruct ()
		: EquipAbilityID(-1)
		, AbilityCount(0)
	{
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="EquipAbility")
	int32 EquipAbilityID = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="EquipAbility")
	int32 AbilityCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="EquipAbility")
	TArray<FKREquipAbilityModifierRow> AbilityModifiers;

	virtual uint32 GetKey() const override
	{
		return EquipAbilityID;
	}
};
