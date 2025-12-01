#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Data/SubQuestDataStruct.h"
#include "SubQuestEvalData.generated.h"

USTRUCT(BlueprintType)
struct KORAPROJECT_API FSubQuestEvalData
{
	GENERATED_BODY()
	
public:
	FSubQuestEvalData()
	{
	}
	
	UPROPERTY(BlueprintReadOnly, Category = SubQuest)
	FSubQuestEvalDataStruct EvalData;

	UPROPERTY(BlueprintReadOnly, Category = SubQuest)
	int32 CurrentProgress;

	UPROPERTY(BlueprintReadOnly, Category = SubQuest)
	float ElapsedTime;

	UPROPERTY(BlueprintReadOnly, Category = SubQuest)
	bool bIsActive;

	UPROPERTY(BlueprintReadOnly, Category = SubQuest)
	bool bFailQuestOnFailure;

	void Init(const FSubQuestDataStruct& SubQuestData, int32 Order);
	void AddProgress(int32 Amount = 1);
	void SubtractProgress(int32 Amount = 1);
	float GetProgressRatio() const;
	float GetRemainingTime() const;

	FORCEINLINE bool IsActive() const { return bIsActive; }
	FORCEINLINE bool IsCompleted() const { return bIsActive && CurrentProgress >= EvalData.RequiredCount; }

	FORCEINLINE bool IsExistLimitTime() const { return EvalData.TimeLimit > 0.f; }
};
