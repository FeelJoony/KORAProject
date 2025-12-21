#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Data/DataAssets/KRWorldEventData.h"
#include "KRWorldEventComponent.generated.h"

class UKRSoundSubsystem;
class UKREffectSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWorldEventTriggered, FGameplayTag, EventTag);

UCLASS(ClassGroup = (KR), meta = (BlueprintSpawnableComponent))
class KORAPROJECT_API UKRWorldEventComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UKRWorldEventComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	// 월드 이벤트 트리거
	UFUNCTION(BlueprintCallable, Category = "KR|WorldEvent")
	bool TriggerWorldEvent(const FGameplayTag& EventTag, AActor* Instigator = nullptr);

	// 월드 이벤트 데이터로 트리거
	UFUNCTION(BlueprintCallable, Category = "KR|WorldEvent")
	bool TriggerWorldEventFromData(UKRWorldEventData* EventData, AActor* Instigator = nullptr);

	// 월드 이벤트 종료
	UFUNCTION(BlueprintCallable, Category = "KR|WorldEvent")
	void EndWorldEvent(const FGameplayTag& EventTag);

	// 월드 이벤트 데이터 등록
	UFUNCTION(BlueprintCallable, Category = "KR|WorldEvent")
	void RegisterWorldEventData(UKRWorldEventData* EventData);

	// 월드 이벤트 데이터 가져오기
	UFUNCTION(BlueprintCallable, Category = "KR|WorldEvent")
	UKRWorldEventData* GetWorldEventData(const FGameplayTag& EventTag) const;

	UPROPERTY(BlueprintAssignable, Category = "KR|WorldEvent")
	FOnWorldEventTriggered OnWorldEventTriggered;

protected:
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UKRWorldEventData>> RegisteredEvents;

	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UAudioComponent>> ActiveEventSounds;

	UPROPERTY()
	TMap<FGameplayTag, int32> EventTriggerCounts;

	// 마지막 트리거 시간
	UPROPERTY()
	TMap<FGameplayTag, float> LastTriggerTimes;

private:
    mutable TWeakObjectPtr<UKRSoundSubsystem> CachedSoundSubsystem;
    mutable TWeakObjectPtr<UKREffectSubsystem> CachedEffectSubsystem;
    
    UKRSoundSubsystem* GetSoundSubsystem() const;
    UKREffectSubsystem* GetEffectSubsystem() const;

	bool CanTriggerEvent(const UKRWorldEventData* EventData) const;
	void PlayEventEffects(const UKRWorldEventData* EventData);
	void ApplyGameplayEffects(const UKRWorldEventData* EventData, AActor* Target);
};
