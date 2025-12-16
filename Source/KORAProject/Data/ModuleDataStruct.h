#pragma once

#include "CoreMinimal.h"
#include "Interface/TableKey.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Interface/TableKey.h"
#include "ModuleDataStruct.generated.h"

class UTexture2D;

UENUM(BlueprintType)
enum class EModuleSlotBit : uint8
{
	None = 0,
	Sword = 1 << 0,
	Gun = 1 << 1,
};
ENUM_CLASS_FLAGS(EModuleSlotBit)

USTRUCT(BlueprintType)
struct KORAPROJECT_API FModuleDataStruct : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()

	FModuleDataStruct()
		: GroupID(-1)
		, ModuleItemTag(FGameplayTag())
		, ModuleSlotBit(EModuleSlotBit::None)
		, EquipAbilityID(-1)
	{}

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Identity")
	int32 GroupID;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Logic")
	FGameplayTag ModuleItemTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Logic")
	EModuleSlotBit ModuleSlotBit;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Logic")
	int32 EquipAbilityID;

public:
	virtual uint32 GetKey() const override
	{
		return GroupID;
	}
};