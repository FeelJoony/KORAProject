#pragma once

#include "CoreMinimal.h"
#include "Interface/TableKey.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Interface/TableKey.h"
#include "ModuleDataStruct.generated.h"

class UTexture2D;

USTRUCT(BlueprintType)
struct KORAPROJECT_API FModuleDataStruct : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()

	FModuleDataStruct()
		: GroupID(-1)
		, ModuleItemTag(FGameplayTag())
		, SlotTag(FGameplayTag())
		, EquipAbilityID(-1)
	{}

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Identity")
	int32 GroupID;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Logic")
	FGameplayTag ModuleItemTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Logic")
	FGameplayTag SlotTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Logic")
	int32 EquipAbilityID;

public:
	virtual uint32 GetKey() const override
	{
		return GetTypeHash(ModuleItemTag);
	}
};