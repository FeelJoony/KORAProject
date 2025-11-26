#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Interface/TableKey.h"
#include "WeaponDataStruct.generated.h"

USTRUCT(BlueprintType)
struct KORAPROJECT_API FWeaponDataStruct : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()
	
	FWeaponDataStruct()
		: GroupID(-1)
		, Atk(0)
		, CritChance(0.f)
		, CritMulti(0.f)
		, Range(0)
		, Accuracy(0)
		, Recoil(0)
		, Capacity(0)
		, ReloadTime(0.f)
		, AttackSpeed(0.f)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	int32 GroupID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	int32 Atk;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	float CritChance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	float CritMulti;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	int32 Range;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	int32 Accuracy;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	int32 Recoil;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	int32 Capacity;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	float ReloadTime;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	float AttackSpeed;

	virtual uint32 GetKey() const override
	{
		return GetTypeHash(GroupID);
	}
};
