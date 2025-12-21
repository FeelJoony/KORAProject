#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Sound/SoundBase.h"
#include "KRSoundDefinition.generated.h"

/**
 * 사운드 재생 조건 및 제약사항
 */
USTRUCT(BlueprintType)
struct FKRSoundPlayCondition
{
	GENERATED_BODY()

	// 동시 재생 가능한 최대 인스턴스 수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Condition")
	int32 MaxConcurrentInstances = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Condition")
	float CooldownDuration = 0.0f;

	// 최소 재생 간격 (초)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Condition")
	float MinTimeBetweenPlays = 0.0f;

	// 우선순위 (높을수록 우선)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Condition")
	int32 Priority = 0;
};

/**
 * 사운드 볼륨 및 피치 설정
 */
USTRUCT(BlueprintType)
struct FKRSoundVolumeSettings
{
	GENERATED_BODY()

	// 기본 볼륨
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Volume", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float BaseVolume = 1.0f;

	// 거리에 따른 볼륨 감쇠 곡선
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Volume")
	FRuntimeFloatCurve VolumeDistanceCurve;

	// 기본 피치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pitch", meta = (ClampMin = "0.1", ClampMax = "3.0"))
	float BasePitch = 1.0f;

	// 피치 변조 범위 (랜덤성 - 발자국)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pitch")
	FVector2D PitchModulationRange = FVector2D(0.95f, 1.05f);
};

/**
 * 사운드 정의 - 전투, 환경, 상호작용 등의 사운드를 정의
 */
UCLASS(BlueprintType)
class KORAPROJECT_API UKRSoundDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// 사운드 식별 태그
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	FGameplayTag SoundTag;

	// 재생할 사운드
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	TSoftObjectPtr<USoundBase> Sound;

	// 사운드가 속한 카테고리 (예: Combat.Sword, Environment.Water)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	FGameplayTagContainer CategoryTags;

	// 볼륨 및 피치 설정
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FKRSoundVolumeSettings VolumeSettings;

	// 재생 조건
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FKRSoundPlayCondition PlayCondition;

	// 감쇠 시작 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attenuation")
	float AttenuationStartDistance = 400.0f;

	// 최대 청취 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attenuation")
	float MaxAudibleDistance = 5000.0f;
};
