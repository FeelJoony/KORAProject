#pragma once

#include "CoreMinimal.h"
#include "KREquipItemData.h"
#include "KRWeaponItemData.generated.h"

USTRUCT(BlueprintType)
struct KORAPROJECT_API FKRWeaponItemData : public FKREquipItemData
{
	GENERATED_BODY()

public:
	FKRWeaponItemData()
		: BaseATK(0), BaseCritChance(0.f), BaseCritMulti(1.f),
		BaseRange(1000.f), BaseAccuracy(1.f), BaseRecoil(0.f),
		BaseCapacity(1), AttackSpeed(1.f),
		ReloadTime(1.f), BaseEnforceCost(0)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 BaseATK;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float BaseCritChance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float BaseCritMulti;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float BaseRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float BaseAccuracy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float BaseRecoil;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 BaseCapacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float AttackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float ReloadTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 BaseEnforceCost;
};