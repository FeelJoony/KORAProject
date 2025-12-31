#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Interface/TableKey.h"
#include "SubQuestDataStruct.generated.h"

USTRUCT(BlueprintType)
struct FSubQuestEvalDataStruct
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SubQuest)
	int32 OrderIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SubQuest)
	FGameplayTag ObjectiveTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SubQuest)
	FText SubQuestName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SubQuest)
	FText Description;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SubQuest)
	int32 RequiredCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SubQuest)
	float TimeLimit;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SubQuest)
	FName UIRowName;
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
	int32 GetEvalDataCount() const { return EvalDatas.Num(); }
};
