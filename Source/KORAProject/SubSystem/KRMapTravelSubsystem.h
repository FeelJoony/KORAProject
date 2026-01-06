#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "KRMapTravelSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnMapTravelStarted);
DECLARE_MULTICAST_DELEGATE(FOnMapTravelCompleted);

/**
 * 맵 전환을 중앙에서 관리하는 서브시스템
 * - 페이드 효과, UI 메시지, 사운드를 통합 처리
 */
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

	// 레벨 로드 완료 감지용
	void OnWorldInitialized(UWorld* World, const UWorld::InitializationValues IVS);
	FDelegateHandle WorldInitHandle;

	// 현재 전환 데이터 저장
	FName PendingLevelNameKey;
	FGameplayTag PendingLevelSoundTag;
	float PendingDisplayDuration;
	bool bIsTransitioning;
	float FadeInDuration = 1.5f;
	float FadeOutDuration = 1.0f;
};
