#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Interface/TableKey.h"
#include "ItemDataStruct.generated.h"

USTRUCT(BlueprintType)
struct KORAPROJECT_API FItemDataStruct : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()

	FItemDataStruct()
			: Index(0)
			, EquipID(-1)
			, AbilityID(-1)
			, ConsumeID(-1)
			, MaterialID(-1)
			, QuestID(-1)
			, TypeTag(FGameplayTag())
			, RarityTag(FGameplayTag())
			, DisplayNameKey(NAME_None)
			, DescriptionKey(NAME_None)
			, Icon(nullptr)
	{}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	int32 Index;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	FGameplayTag TypeTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	FGameplayTag RarityTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	TArray<FGameplayTag> AbilityTags;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	FName DisplayNameKey;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	FName DescriptionKey;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	TSoftObjectPtr<UTexture2D> Icon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	int32 EquipID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	int32 AbilityID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	int32 ConsumeID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	int32 MaterialID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	int32 QuestID;

	virtual uint32 GetKey() const override
	{
		return GetTypeHash(TypeTag);
	}
};
