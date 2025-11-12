#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Engine/Texture2D.h"
#include "Interface/TableKey.h"
#include "GameplayTagsManager.h"
#include "ConsumeItemDataStruct.generated.h"

UENUM(BlueprintType)
enum class EConsumeType : uint8
{
	Consume1,
};

USTRUCT(BlueprintType)
struct KORAPROJECT_API FConsumeItemDataStruct : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()

public:
	FConsumeItemDataStruct()
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool Tag")
	FGameplayTag PoolTag;

	virtual uint32 GetKey() const override
	{
		return Index;
	}
};

