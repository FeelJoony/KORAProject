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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	int32 Index;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	FGameplayTag TypeTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	FGameplayTag RarityTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	TArray<FGameplayTag> AbilityTags;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	FName DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	FName Description;
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
