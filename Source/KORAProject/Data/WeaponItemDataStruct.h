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
	Weapon1,
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
		, BaseCapacity(1)
		, AttackSpeed(1.f)
		, ReloadTime(1.f)
		, BaseEnforceCost(0)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	int32 Index;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tag")
	FGameplayTag TypeTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tag")
	FGameplayTag RarityTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tag")
	TArray<FGameplayTag> AbilityTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSoftObjectPtr<UTexture2D> ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
	int32 BasePrice;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stack")
	int32 StackMax;

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

	virtual int32 GetKey() const override
	{
		return Index;
	}
};
