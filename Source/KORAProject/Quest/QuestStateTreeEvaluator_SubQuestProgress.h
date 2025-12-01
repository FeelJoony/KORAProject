#pragma once

#include "CoreMinimal.h"
#include "StateTreeEvaluatorBase.h"
#include "GameplayTagContainer.h"
#include "StateTreeExecutionTypes.h"
#include "SubQuestEvalData.h"
#include "Data/SubQuestDataStruct.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "QuestStateTreeEvaluator_SubQuestProgress.generated.h"

USTRUCT(BlueprintType)
struct FQuestGameplayMessage
{
	GENERATED_BODY()
	
	int32 QuestCount;
};

USTRUCT(BlueprintType)
struct KORAPROJECT_API FQuestStateTreeEvaluator_SubQuestProgressInstanceData
{
	GENERATED_BODY()

	UPROPERTY()
	bool bQuestFailed = false;

	void Register(class UKRQuestComponent* Component);
	
	void NotifyProgress(FGameplayTag TargetTag, int32 Count);
	void SetSubQuestActive(int32 SubQuestDataKey, bool bActive);
	bool IsAllSubQuestsCompleted() const;

	void AddEvalData(const FSubQuestDataStruct& SubQuestData, int32 Order);
	const FSubQuestEvalData* GetEvalData(int32 Order) const;

	int32 CurrentOrder = 0;

protected:
	UPROPERTY()
	TObjectPtr<class UKRQuestComponent> QuestComponent;

	UPROPERTY()
	TMap<int32, FSubQuestEvalData> SubQuestProgressMap;
	
};

using FInstanceData = FQuestStateTreeEvaluator_SubQuestProgressInstanceData;

UCLASS()
class UGameplayMessageFunction : public UObject
{
	GENERATED_BODY()

public:
	void SetEvaluatorInstance(FInstanceData* Data)
	{
		this->InstanceData = Data;
	}
	
	void RegisterAll(class UKRQuestComponent* QuestComponent);

	void UnregisterAll();
	
	UFUNCTION(BlueprintCallable, Category = Quest)
	void ReceiveQuestMessageCallback(FGameplayTag ObjectiveTag, const FQuestGameplayMessage& Message);
	
private:
	FInstanceData* InstanceData;

	struct FQuestStateMessageHandleData
	{
		FQuestStateMessageHandleData(FGameplayMessageListenerHandle InHandle) : MessageHandle(InHandle)		{ }
		
		FGameplayMessageListenerHandle MessageHandle;
	};

	TArray<FQuestStateMessageHandleData> QuestStateMessageDatas;
};

USTRUCT(meta = (DisplayName = "SubQuest Progress Evaluator"))
struct KORAPROJECT_API FQuestStateTreeEvaluator_SubQuestProgress : public FStateTreeEvaluatorCommonBase
{
	GENERATED_BODY()

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceData::StaticStruct(); }

	virtual void TreeStart(FStateTreeExecutionContext& Context) const override;

	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

	virtual void TreeStop(FStateTreeExecutionContext& Context) const override;

protected:
	UPROPERTY()
	mutable TObjectPtr<UGameplayMessageFunction> MessageFunction;
};
