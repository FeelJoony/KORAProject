#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "KRStarDashData.generated.h"

class UAnimMontage;
class UCameraShakeBase;
class UKRCameraMode;

USTRUCT(BlueprintType)
struct FKRDashPointData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	FVector LocalOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	bool bIsOuterPoint = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect", meta = (Categories = "Effect"))
	FGameplayTag PointEffectTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (Categories = "Sound"))
	FGameplayTag PointSoundTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing", meta = (ClampMin = "0.0"))
	float DashDurationOverride = 0.0f;
};

USTRUCT(BlueprintType)
struct FKRStarDashSoundSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundTag", meta = (Categories = "Sound"))
	FGameplayTag DashStartSoundTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundTag", meta = (Categories = "Sound"))
	FGameplayTag DashLoopSoundTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundTag", meta = (Categories = "Sound"))
	FGameplayTag SlashSoundTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundTag", meta = (Categories = "Sound"))
	FGameplayTag FinishSoundTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundTag", meta = (Categories = "Sound"))
	FGameplayTag StarCompleteSoundTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayCue", meta = (Categories = "GameplayCue"))
	FGameplayTag DashCueTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayCue", meta = (Categories = "GameplayCue"))
	FGameplayTag SlashCueTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayCue", meta = (Categories = "GameplayCue"))
	FGameplayTag FinishCueTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayCue", meta = (Categories = "GameplayCue"))
	FGameplayTag StarCompleteCueTag;
};

USTRUCT(BlueprintType)
struct FKRStarDashEffectSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EffectTag", meta = (Categories = "Effect"))
	FGameplayTag SlashLineEffectTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EffectTag", meta = (Categories = "Effect"))
	FGameplayTag LightningEffectTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EffectTag", meta = (Categories = "Effect"))
	FGameplayTag AfterImageEffectTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EffectTag", meta = (Categories = "Effect"))
	FGameplayTag StarCompleteEffectTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EffectTag", meta = (Categories = "Effect"))
	FGameplayTag FinishEffectTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineParams")
	FLinearColor LineColor = FLinearColor(1.0f, 0.8f, 0.2f, 1.0f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineParams", meta = (ClampMin = "1.0", ClampMax = "50.0"))
	float LineWidth = 15.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineParams", meta = (ClampMin = "0.0", ClampMax = "20.0"))
	float GlowIntensity = 5.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lightning")
	bool bEnableLightning = true;

	/**
	 * Lightning 모드:
	 * 0 = Multi-Segment (휘는 경로)
	 * 1 = SplineVFX BP Actor
	 * 2 = Single (직선)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lightning", meta = (ClampMin = "0", ClampMax = "2"))
	int32 LightningMode = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lightning")
	FLinearColor LightningColor = FLinearColor(0.5f, 0.7f, 1.0f, 1.0f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lightning", meta = (ClampMin = "0.5", ClampMax = "10.0"))
	float LightningIntensity = 3.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AfterImage")
	FLinearColor AfterImageColor = FLinearColor(0.5f, 0.8f, 1.0f, 0.5f);
};

USTRUCT(BlueprintType)
struct FKRStarDashTimingSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing", meta = (ClampMin = "0.01", ClampMax = "1.0"))
	float DashDuration = 0.15f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing", meta = (ClampMin = "0.0", ClampMax = "0.5"))
	float DashInterval = 0.05f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float FinishDelay = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing", meta = (ClampMin = "0.1", ClampMax = "3.0"))
	float LineDrawDuration = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing", meta = (ClampMin = "0.0", ClampMax = "0.5"))
	float HitStopDuration = 0.1f;
};

USTRUCT(BlueprintType)
struct FKRStarDashPatternSettings
{
	GENERATED_BODY()

	/**
	 * 패턴 타입:
	 * 0 = 10포인트 별 (외곽+내부, 별 윤곽선)
	 * 1 = 5포인트 별 (꼭짓점 스킵, 교차선)
	 * 2 = 커스텀 (CustomPoints 사용)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern", meta = (ClampMin = "0", ClampMax = "2"))
	int32 PatternType = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern", meta = (ClampMin = "50.0", ClampMax = "1000.0"))
	float OuterRadius = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern", meta = (ClampMin = "0.0", ClampMax = "500.0"))
	float InnerRadius = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	float HeightOffset = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	bool bHorizontal = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	float StartAngleOffset = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern", meta = (EditCondition = "PatternType == 2"))
	TArray<FKRDashPointData> CustomPoints; // When Custom Points
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	bool bReturnToStart = true;
};

USTRUCT(BlueprintType)
struct FKRStarDashMontageSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	TObjectPtr<UAnimMontage> DashMontage = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	TObjectPtr<UAnimMontage> FinishMontage = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", meta = (ClampMin = "0.5", ClampMax = "3.0"))
	float DashMontagePlayRate = 1.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", meta = (ClampMin = "0.5", ClampMax = "2.0"))
	float FinishMontagePlayRate = 1.0f;
};

USTRUCT(BlueprintType)
struct FKRStarDashVisualSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DashOpacity = 0.3f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	bool bUseVisibilityHide = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	bool bUseDitheredOpacity = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual", meta = (ClampMin = "0", ClampMax = "7"))
	int32 OpacityPrimitiveDataIndex = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	FName OpacityParameterName = FName("Opacity");
};


USTRUCT(BlueprintType)
struct FKRStarDashDamageSettings
{
	GENERATED_BODY()

	/** 각 대시당 데미지 배율 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage", meta = (ClampMin = "0.0", ClampMax = "5.0"))
	float DashDamageMultiplier = 0.5f;

	/** 피니시 공격 데미지 배율 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage", meta = (ClampMin = "0.0", ClampMax = "10.0"))
	float FinishDamageMultiplier = 2.0f;
};

USTRUCT(BlueprintType)
struct FKRStarDashCameraSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraMode")
	TSubclassOf<UKRCameraMode> StarDashCameraMode = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraMode")
	bool bEnableCameraMode = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position", meta = (ClampMin = "0.0", ClampMax = "1000.0"))
	float CameraHeightOffset = 400.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position", meta = (ClampMin = "100.0", ClampMax = "1500.0"))
	float CameraBackwardDistance = 600.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position", meta = (ClampMin = "0.0", ClampMax = "500.0"))
	float CameraLookAtHeightOffset = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position", meta = (ClampMin = "0.1", ClampMax = "20.0"))
	float CameraInterpSpeed = 8.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FOV", meta = (ClampMin = "60.0", ClampMax = "120.0"))
	float SkillFOV = 90.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FOV", meta = (ClampMin = "0.1", ClampMax = "10.0"))
	float FOVInterpSpeed = 5.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake")
	TSubclassOf<UCameraShakeBase> FinishCameraShake = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float CameraShakeScale = 1.0f;
};

UCLASS(BlueprintType)
class KORAPROJECT_API UKRStarDashData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info", meta = (MultiLine = true))
	FText Description;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pattern")
	FKRStarDashPatternSettings Pattern;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timing")
	FKRStarDashTimingSettings Timing;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	FKRStarDashEffectSettings Effects;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	FKRStarDashSoundSettings Sound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	FKRStarDashMontageSettings Montage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	FKRStarDashVisualSettings Visual;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	FKRStarDashCameraSettings Camera;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	FKRStarDashDamageSettings Damage;
	
	UFUNCTION(BlueprintPure, Category = "StarDash")
	int32 GetDashCount() const
	{
		switch (Pattern.PatternType)
		{
		case 0: return 10;  // 10포인트 별
		case 1: return 5;   // 5포인트 별
		case 2: return Pattern.CustomPoints.Num();  // 커스텀
		default: return 5;
		}
	}

	UFUNCTION(BlueprintPure, Category = "StarDash")
	int32 GetLineCount() const
	{
		int32 DashCount = GetDashCount();
		return Pattern.bReturnToStart ? DashCount : FMath::Max(0, DashCount - 1);
	}

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("StarDashData"), GetFName());
	}
};
