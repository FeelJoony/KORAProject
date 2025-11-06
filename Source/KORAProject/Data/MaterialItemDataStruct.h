#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Engine/Texture2D.h"
#include "Interface/TableKey.h"
#include "GameplayTagsManager.h"
#include "MaterialItemDataStruct.generated.h"

UENUM(BlueprintType)
enum class EMaterialType : uint8
{
	Material1,
};

USTRUCT(BlueprintType)
struct KORAPROJECT_API FMaterialItemDataStruct : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()

public:
	FMaterialItemDataStruct()
		: Index(0), BasePrice(0), StackMax(1)
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

	virtual int32 GetKey() const override
	{
		return Index;
	}
};


