#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "KRMapTravelSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnMapTravelStarted);
DECLARE_MULTICAST_DELEGATE(FOnMapTravelCompleted);

UCLASS()
class KORAPROJECT_API UKRMapTravelSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	static UKRMapTravelSubsystem& Get(const UObject* WorldContextObject);

	// ObjectiveTag도 함께 받아서 전환 데이터 로드
	UFUNCTION(BlueprintCallable, Category = "MapTravel|Gameplay")
	void TravelToExperience(const FString& UserFacingPath, FGameplayTag ActivationTag);

	// 델리게이트
	FOnMapTravelStarted OnMapTravelStarted;
	FOnMapTravelCompleted OnMapTravelCompleted;

private:
	void StartTransitionSequence(const class UKRUserFacingExperience* Experience,
		const struct FLevelTransitionDataStruct& TransitionData,
		FGameplayTag ActivationTag);

	void OnLevelLoadComplete(FName StringTableKey, FGameplayTag SoundTag, float DisplayDuration);
	void ExecuteServerTravel(const class UKRUserFacingExperience* Experience);
	void BeginAsyncLoadingCheck();
	void CheckAsyncLoading();
	void OnAsyncLoadingFinished();
	void OnMinLoadingTimeElapsed();
	void TryFinishLoading();
	
	// 레벨 로드 완료 감지용
	void OnWorldInitialized(UWorld* World, const UWorld::InitializationValues IVS);
	FDelegateHandle WorldInitHandle;

	FName PendingLevelNameKey;
	FGameplayTag PendingLevelSoundTag;
	
	UPROPERTY(EditAnywhere, Category = "Loading")
	float PendingDisplayDuration = 3.0f;
	UPROPERTY(EditAnywhere, Category = "Loading")
	float MinLoadingDuration = 3.0f;

	bool bIsTransitioning = false;
	bool bMinLoadingTimeElapsed = false;
	bool bAsyncLoadingFinished = false;
	bool bWaitingForAsyncLoading = false;

	FTimerHandle MinLoadingTimerHandle;
	FTimerHandle AsyncLoadingCheckTimer;
};
