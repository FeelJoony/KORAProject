#pragma once

#include "CoreMinimal.h"
#include "Data/QuestDataStruct.h"
#include "Data/SubQuestDataStruct.h"
#include "SubQuestEvalData.h"
#include "KRQuestInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogQuestInstance, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestAccepted, int32, QuestIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestCompleted, int32, QuestIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestFailed, int32, QuestIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSubQuestProgress, int32, QuestIndex, int32, SubQuestDataKey, const struct FSubQuestEvalData&, EvalData);

UCLASS(BlueprintType)
class KORAPROJECT_API UKRQuestInstance : public UObject
{
	GENERATED_BODY()

public:
	UKRQuestInstance();

	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnQuestAccepted OnQuestAccepted;

	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnQuestCompleted OnQuestCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnQuestFailed OnQuestFailed;

	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnSubQuestProgress OnSubQuestProgress;

	void Initialize(int32 QuestIndex);

	UFUNCTION(BlueprintCallable, Category = Quest)
	void StartQuest();

	void TickQuest(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = Quest)
	void CompleteQuest();

	UFUNCTION(BlueprintCallable, Category = Quest)
	void FailQuest();
	
	UFUNCTION(BlueprintPure, Category = Quest)
	EQuestState GetQuestState() const { return CurrentState; }

	UFUNCTION(BlueprintPure, Category = Quest)
	int32 GetQuestIndex() const { return CurrentQuestData.GetKey(); }

	UFUNCTION(BlueprintPure, Category = Quest)
	const FQuestDataStruct& GetQuestData() const { return CurrentQuestData; }

	UFUNCTION(BlueprintPure, Category = Quest)
	const FSubQuestDataStruct& GetSubQuestData() const { return CurrentSubQuestData; }

	UFUNCTION(BlueprintPure, Category = Quest)
	bool IsQuestCompleted() const { return CurrentState == EQuestState::Completed; }

	UFUNCTION(BlueprintPure, Category = Qeust)
	bool IsSubQuestCompleted(int32 Order) { return SubQuestProgressMap.Find(Order)->IsCompleted(); }

	UFUNCTION(BlueprintPure, Category = Quest)
	const FSubQuestEvalDataStruct& GetSubQuestEvalDataStruct() const { return CurrentSubQuestData.EvalDatas[CurrentSubOrder - 1]; }

	UFUNCTION(BlueprintPure, Category = Quest)
	const FSubQuestEvalData& GetSubQuestEvalData() const { return *SubQuestProgressMap.Find(CurrentSubOrder); }
	
	void AddCount(int32 Amount);
	void SetNextQuest();
	

protected:
	UPROPERTY(BlueprintReadOnly, Category = Quest)
	FQuestDataStruct CurrentQuestData;
	
	UPROPERTY(BlueprintReadOnly, Category = Quest)
	FSubQuestDataStruct CurrentSubQuestData;
	
	UPROPERTY(BlueprintReadOnly, Category = Quest)
	TObjectPtr<class AKRQuestActor> OwningActor;
	
	UPROPERTY(BlueprintReadOnly, Category = Quest)
	int32 CurrentSubOrder;
	
	UPROPERTY(BlueprintReadOnly, Category = Quest)
	EQuestState CurrentState;


private:
	UPROPERTY()
	TMap<int32, FSubQuestEvalData> SubQuestProgressMap;
};
