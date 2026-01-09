#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "KRSoundDefinition.h"
#include "KREffectDefinition.h"
#include "KRWorldEventData.generated.h"

/**
 * 월드 이벤트 타입
 */
UENUM(BlueprintType)
enum class EKRWorldEventType : uint8
{
	EnvironmentEnter,
	EnvironmentExit,
	TriggerActivation,
	Interaction,
	Custom
};

/**
 * 월드 이벤트 효과 (사운드, 이펙트)
 */
USTRUCT(BlueprintType)
struct FKRWorldEventEffects
{
	GENERATED_BODY()

	// 사운드 태그
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	FGameplayTag SoundTag;

	// 사운드 루프 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	bool bLoopSound = false;

	// 사운드 페이드 인 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	float SoundFadeInTime = 0.5f;

	// 사운드 페이드 아웃 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	float SoundFadeOutTime = 0.5f;

	// 이펙트 태그
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	FGameplayTag EffectTag;

	// 이펙트 지속 여부 (false면 일회성)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	bool bPersistentEffect = false;

	// 이펙트 지속 시간 (0이면 무한)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects", meta = (EditCondition = "bPersistentEffect"))
	float EffectDuration = 0.0f;

	//BGM 태그
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	FGameplayTag BGMGroupTag;

	// BGM 루프 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	bool bLoopBGM = true;

	// BGM 페이드 인 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	float BGMFadeInTime = 2.f;

	// BGM 페이드 아웃 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	float BGMFadeOutTime = 2.f;
};

/**
 * 월드 이벤트 게임플레이 효과
 */
USTRUCT(BlueprintType)
struct FKRWorldEventGameplayEffects
{
	GENERATED_BODY()

	// 적용할 Gameplay Effect 클래스들
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	TArray<TSubclassOf<class UGameplayEffect>> GameplayEffects;

	// 추가할 태그
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	FGameplayTagContainer GrantedTags;

	// 제거할 태그
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	FGameplayTagContainer RemovedTags;

	// Gameplay Cue 태그 (발동할 Cue)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	FGameplayTag GameplayCueTag;
};

/**
 * 월드 이벤트 트리거 조건
 */
USTRUCT(BlueprintType)
struct FKRWorldEventTriggerCondition
{
	GENERATED_BODY()

	// 트리거 발동에 필요한 태그
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trigger")
	FGameplayTagContainer RequiredTags;

	// 트리거 차단 태그
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trigger")
	FGameplayTagContainer BlockedTags;

	// 최소 트리거 간격 (초)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trigger")
	float MinTriggerInterval = 0.0f;

	// 최대 트리거 횟수 (0이면 무제한)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trigger")
	int32 MaxTriggerCount = 0;

	// 한 번만 트리거 (MaxTriggerCount = 1과 동일)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trigger")
	bool bTriggerOnce = false;
};

/**
 * 월드 이벤트 데이터 - 환경 이벤트, 트리거 등을 정의
 */
UCLASS(BlueprintType)
class KORAPROJECT_API UKRWorldEventData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// 월드 이벤트 식별 태그
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WorldEvent")
	FGameplayTag EventTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WorldEvent")
	EKRWorldEventType EventType = EKRWorldEventType::EnvironmentEnter;

	// 이벤트 이름
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WorldEvent")
	FText EventName;

	// 이벤트 설명
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WorldEvent")
	FText EventDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	FKRWorldEventEffects Effects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	FKRWorldEventGameplayEffects GameplayEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trigger")
	FKRWorldEventTriggerCondition TriggerCondition;

	// 우선순위
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	int32 Priority = 0;

	// 종료 시 실행할 이벤트 태그 (환경 퇴장 시 등)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FGameplayTag ExitEventTag;
};
