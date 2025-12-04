#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Interface/TableKey.h"
#include "SubQuestDataStruct.generated.h"

USTRUCT(BlueprintType)
struct FSubQuestEvalDataStruct
{
	GENERATED_BODY()
	
	int32 OrderIndex;
	
	FGameplayTag ObjectiveTag;
	
	FText SubQuestName;
	
	FText Description;
	
	int32 RequiredCount;
	
	float TimeLimit;
};

USTRUCT(BlueprintType)
struct KORAPROJECT_API FSubQuestDataStruct : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()

	FSubQuestDataStruct()
		: GroupIndex(0)
		, EvalDatas({})
	{
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SubQuest)
	int32 GroupIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SubQuest)
	TArray<FSubQuestEvalDataStruct> EvalDatas;

	virtual uint32 GetKey() const override
	{
		return GroupIndex;
	}

	const FSubQuestEvalDataStruct& GetEvalData(int Order) const { return EvalDatas[Order - 1]; }
};
