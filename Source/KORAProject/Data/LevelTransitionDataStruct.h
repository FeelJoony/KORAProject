#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Interface/TableKey.h"
#include "LevelTransitionDataStruct.generated.h"

USTRUCT(BlueprintType)
struct KORAPROJECT_API FLevelTransitionDataStruct : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()

	FLevelTransitionDataStruct()
		: ObjectiveTag(FGameplayTag::EmptyTag)
		, StringTableKey(TEXT(""))
		, LevelEnterSoundTag(FGameplayTag::EmptyTag)
	{}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FGameplayTag ObjectiveTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FName StringTableKey;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	FGameplayTag LevelEnterSoundTag;

	virtual uint32 GetKey() const override
	{
		return GetTypeHash(ObjectiveTag);
	}
};
