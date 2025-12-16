#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Interface/TableKey.h"
#include "EquipDataStruct.generated.h"

USTRUCT(BlueprintType)
struct KORAPROJECT_API FEquipDataStruct : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()
	
	FEquipDataStruct()
		: GroupID(-1)
		, EquipItemTag(FGameplayTag::EmptyTag)
		, SlotTag(FGameplayTag::EmptyTag)
		, EquipmentMesh(nullptr)
		, OverrideMaterials({})
		, ModuleSlotTag(FGameplayTag())
	{
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equip)
	int32 GroupID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equip)
	FGameplayTag EquipItemTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equip)
	FGameplayTag SlotTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equip)
	TSoftObjectPtr<UStaticMesh> EquipmentMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equip)
	TArray<TSoftObjectPtr<UMaterialInterface>> OverrideMaterials;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equip)
	FGameplayTag ModuleSlotTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equip)
	int32 EquipAbilityID;

	virtual uint32 GetKey() const override { return GroupID; }
};
