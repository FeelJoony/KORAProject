#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Engine/Texture2D.h"
#include "Interface/TableKey.h"
#include "GameplayTagsManager.h"
#include "WeaponItemDataStruct.generated.h"

UENUM(BlueprintType)
enum class EWeaponItemType : uint8
{
	None,
	Sample1,
	Sample2,
	Sample3,
	Sample4,
	Sample5
};

USTRUCT(BlueprintType)
struct KORAPROJECT_API FWeaponItemDataStruct : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()

public:
	FWeaponItemDataStruct()
		: Index(1)
		, BasePrice(0)
		, StackMax(1)
		, BaseATK(0)
		, BaseCritChance(0.f)
		, BaseCritMulti(1.f)
		, BaseRange(1000.f)
		, BaseAccuracy(1.f)
		, BaseRecoil(0.f)
		, BaseCapacity(30)
		, BaseEnforceCost(0)
		, ReloadTime(1.f)
		, AttackSpeed(1.f)
	{

	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ID")
	int32 Index;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tags")
	FGameplayTag TypeTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tags")
	FGameplayTag RarityTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tags")
	TArray<FGameplayTag> AbilityTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSoftObjectPtr<UTexture2D> ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Economy")
	int32 BasePrice;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stack")
	int32 StackMax;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	int32 BaseATK;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	float BaseCritChance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	float BaseCritMulti;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	float BaseRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	float BaseAccuracy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	float BaseRecoil;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	int32 BaseCapacity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	int32 BaseEnforceCost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	float ReloadTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	float AttackSpeed;

	virtual int32 GetKey() const override
	{
		return Index;
	}
};
