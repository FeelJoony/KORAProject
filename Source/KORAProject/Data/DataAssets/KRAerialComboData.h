#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "KRAerialComboData.generated.h"

class UAnimMontage;
class UCameraShakeBase;
class UGameplayEffect;
class UKRCameraMode;

USTRUCT(BlueprintType)
struct FKRAerialComboDamageSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage", meta = (ClampMin = "0.5", ClampMax = "3.0"))
	float LaunchDamageMultiplier = 1.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage", meta = (ClampMin = "1.0", ClampMax = "5.0"))
	float SlamDamageMultiplier = 2.5f;

	/** 데미지 GameplayEffect 클래스 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass = nullptr;

	/** 런칭 GameplayEffect (적에게 적용, 공중에 띄움) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	TSubclassOf<UGameplayEffect> LaunchEffectClass = nullptr;
};

USTRUCT(BlueprintType)
struct FKRAerialComboMontageSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	TObjectPtr<UAnimMontage> AerialMontage = nullptr;
};

USTRUCT(BlueprintType)
struct FKRAerialComboEffectSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EffectTag", meta = (Categories = "Effect"))
	FGameplayTag LaunchWindEffectTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EffectTag", meta = (Categories = "Effect"))
	FGameplayTag LineTrailEffectTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EffectTag", meta = (Categories = "Effect"))
	FGameplayTag ImpactEffectTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EffectTag", meta = (Categories = "Effect"))
	FGameplayTag FinishEffectTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	FLinearColor EffectColor = FLinearColor(0.8f, 0.2f, 0.1f, 1.0f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	FLinearColor AfterImageColor = FLinearColor(0.8f, 0.2f, 0.1f, 0.5f);
};

USTRUCT(BlueprintType)
struct FKRAerialComboSoundSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundTag", meta = (Categories = "Sound"))
	FGameplayTag LaunchSoundTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundTag", meta = (Categories = "Sound"))
	FGameplayTag DashSoundTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundTag", meta = (Categories = "Sound"))
	FGameplayTag HitSoundTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundTag", meta = (Categories = "Sound"))
	FGameplayTag FinishSoundTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayCue", meta = (Categories = "GameplayCue"))
	FGameplayTag LaunchCueTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayCue", meta = (Categories = "GameplayCue"))
	FGameplayTag DashCueTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayCue", meta = (Categories = "GameplayCue"))
	FGameplayTag HitCueTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayCue", meta = (Categories = "GameplayCue"))
	FGameplayTag FinishCueTag;
};

USTRUCT(BlueprintType)
struct FKRAerialComboTargetingSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	bool bRequiresTarget = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting", meta = (ClampMin = "100.0", ClampMax = "1000.0"))
	float TargetSearchRadius = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting", meta = (ClampMin = "30.0", ClampMax = "180.0"))
	float TargetSearchAngle = 90.0f;
};

USTRUCT(BlueprintType)
struct FKRAerialComboCameraSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraMode")
	TSubclassOf<UKRCameraMode> AerialComboCameraMode = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraMode")
	bool bEnableCameraMode = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake")
	TSubclassOf<UCameraShakeBase> LaunchCameraShake = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake")
	TSubclassOf<UCameraShakeBase> SlamCameraShake = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake")
	TSubclassOf<UCameraShakeBase> ImpactCameraShake = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float CameraShakeScale = 1.0f;
};

UCLASS(BlueprintType)
class KORAPROJECT_API UKRAerialComboData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	FKRAerialComboDamageSettings Damage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	FKRAerialComboMontageSettings Montage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	FKRAerialComboEffectSettings Effects;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	FKRAerialComboSoundSettings Sound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting")
	FKRAerialComboTargetingSettings Targeting;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	FKRAerialComboCameraSettings Camera;
	
	//UFUNCTION(BlueprintPure, Category = "AerialCombo")
	//float GetEstimatedDuration() const
	//{
	//	// Launch + Airborne + Slam + Landing 대략적 시간
	//	return 2.0f + Launch.AirborneHangTime + Slam.HitStopDuration;
	//}
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("AerialComboData"), GetFName());
	}
};
