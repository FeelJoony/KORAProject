// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PlayerStateComponent.h"
#include "GameplayTagContainer.h"
#include "KRQuestComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogQuestComponent, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestAccepted, int32, QuestIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestCompleted, int32, QuestIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestFailed, int32, QuestIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSubQuestProgress, int32, QuestIndex, int32, SubQuestDataKey, const struct FSubQuestEvalData&, EvalData);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KORAPROJECT_API UKRQuestComponent : public UPlayerStateComponent
{
	GENERATED_BODY()

public:
	UKRQuestComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UPROPERTY(BlueprintAssignable, Category = "Quest|Events")
	FOnQuestAccepted OnQuestAccepted;

	UPROPERTY(BlueprintAssignable, Category = "Quest|Events")
	FOnQuestCompleted OnQuestCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Quest|Events")
	FOnQuestFailed OnQuestFailed;

	UPROPERTY(BlueprintAssignable, Category = "Quest|Events")
	FOnSubQuestProgress OnSubQuestProgress;
	
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = Quest)
	bool AcceptQuest(int32 Index);

	UFUNCTION(BlueprintCallable, Category = Quest)
	bool AbandonQuest(int32 Index);

	UFUNCTION(BlueprintCallable, Category = Quest)
	void NotifyObjectiveEvent(FGameplayTag TargetTag, int32 Count = 1);

	UFUNCTION(BlueprintPure, Category = Quest)
	class UKRQuestInstance* GetActiveQuest() const { return ActiveQuest; }

	//UFUNCTION(BlueprintPure, Category = Quest)
	//class UKRQuestInstance* FindActiveQuest(FGameplayTag QuestTag) const;

	UFUNCTION(BlueprintPure, Category = Quest)
	bool IsQuestCompleted(int32 Index) const;

protected:

	UPROPERTY(BlueprintReadOnly, Category = Quest)
	TObjectPtr<class UKRQuestInstance> ActiveQuest;

	UPROPERTY(BlueprintReadOnly, Category = Quest)
	TSet<int32> CompletedQuests;

	UPROPERTY(BlueprintReadOnly, Category = Quest)
	int32 CurrentQuestIndex;

	void CheckQuestCompletion();
};
