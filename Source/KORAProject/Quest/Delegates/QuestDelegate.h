#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "QuestDelegate.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestAccepted, int32, OrderIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestCompleted, int32, OrderIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestFailed, int32, OrderIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestSetNextSub, int32, OrderIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestProgress, int32, OrderIndex, const struct FSubQuestEvalData&, EvalData);

UCLASS()
class KORAPROJECT_API UQuestDelegate : public UObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(class UKRQuestInstance* NewQuestInstance);
	virtual void Reinitialize(class UKRQuestInstance* NewQuestInstance);
	virtual void Uninitialize();

	virtual void OnAcceptedBroadcast(int32 OrderIndex);
	virtual void OnCompletedBroadcast(int32 OrderIndex);
	virtual void OnFailedBroadcast(int32 OrderIndex);
	virtual void OnSetNextSubBroadcast(int32 OrderIndex);
	virtual void OnProgressBroadcast(int32 OrderIndex, const struct FSubQuestEvalData& EvalData);
	
protected:
	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnQuestAccepted OnQuestAccepted;

	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnQuestCompleted OnQuestCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnQuestFailed OnQuestFailed;
	
	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnQuestSetNextSub OnQuestSetNextSub;

	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnQuestProgress OnQuestProgress;

protected:
	UPROPERTY()
	TObjectPtr<class UKRQuestInstance> QuestInstance;
	
};
