#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Interface/TableKey.h"
#include "WeaponEnhanceDataStruct.generated.h"

USTRUCT(BlueprintType)
struct KORAPROJECT_API FWeaponEnhanceDataStruct : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()

	FWeaponEnhanceDataStruct()
			: GroupID(-1)
			, Lv(0)
			, AddAtk(0)
			, AddCritChance(0.f)
			, AddRange(0)
			, Success(0)
			, Cost(0)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	int32 GroupID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	int32 Lv;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	int32 AddAtk;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	float AddCritChance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	int32 AddRange;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	int32 Success;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	int32 Cost;

	virtual uint32 GetKey() const override
	{
		return GetTypeHash(GroupID);
	}
};