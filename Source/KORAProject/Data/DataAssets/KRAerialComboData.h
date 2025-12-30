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
struct FKRAerialComboLaunchSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Launch", meta = (ClampMin = "100.0", ClampMax = "1000.0"))
	float EnemyLaunchHeight = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Launch", meta = (ClampMin = "500.0", ClampMax = "2000.0"))
	float EnemyLaunchVelocity = 1200.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Launch", meta = (ClampMin = "500.0", ClampMax = "2000.0"))
	float PlayerJumpVelocity = 1000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Launch", meta = (ClampMin = "0.1", ClampMax = "1.0"))
	float AirborneHangTime = 0.3f;
};

USTRUCT(BlueprintType)
struct FKRAerialComboSlamSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slam", meta = (ClampMin = "1000.0", ClampMax = "5000.0"))
	float SlamVelocity = 2000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slam", meta = (ClampMin = "100.0", ClampMax = "500.0"))
	float ImpactRadius = 300.0f;

	/** 바운스 높이 (적이 땅에 부딪힐 때) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slam", meta = (ClampMin = "0.0", ClampMax = "300.0"))
	float BounceHeight = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slam", meta = (ClampMin = "0.0", ClampMax = "0.5"))
	float HitStopDuration = 0.15f;
};

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
	TObjectPtr<UAnimMontage> LaunchMontage = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	TObjectPtr<UAnimMontage> RisingMontage = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	TObjectPtr<UAnimMontage> AirborneMontage = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	TObjectPtr<UAnimMontage> SlamMontage = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	TObjectPtr<UAnimMontage> LandingMontage = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage", meta = (ClampMin = "0.5", ClampMax = "2.0"))
	float MontagePlayRate = 1.0f;
};

USTRUCT(BlueprintType)
struct FKRAerialComboEffectSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EffectTag", meta = (Categories = "Effect"))
	FGameplayTag LaunchWindEffectTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EffectTag", meta = (Categories = "Effect"))
	FGameplayTag AfterImageEffectTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EffectTag", meta = (Categories = "Effect"))
	FGameplayTag SlamTrailEffectTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EffectTag", meta = (Categories = "Effect"))
	FGameplayTag ImpactEffectTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EffectTag", meta = (Categories = "Effect"))
	FGameplayTag GroundCrackEffectTag;
	
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
	FGameplayTag WindSoundTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundTag", meta = (Categories = "Sound"))
	FGameplayTag SlamSoundTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundTag", meta = (Categories = "Sound"))
	FGameplayTag ImpactSoundTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayCue", meta = (Categories = "GameplayCue"))
	FGameplayTag LaunchCueTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayCue", meta = (Categories = "GameplayCue"))
	FGameplayTag RisingCueTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayCue", meta = (Categories = "GameplayCue"))
	FGameplayTag SlamCueTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayCue", meta = (Categories = "GameplayCue"))
	FGameplayTag ImpactCueTag;
};

USTRUCT(BlueprintType)
struct FKRAerialComboMotionWarpingSettings
{
	GENERATED_BODY()

	/** 런칭 워프 타겟 이름 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MotionWarping")
	FName LaunchWarpTargetName = FName("LaunchTarget");
	/** 공중 추적 워프 타겟 이름 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MotionWarping")
	FName AirborneWarpTargetName = FName("AirborneTarget");
	/** 슬램 워프 타겟 이름 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MotionWarping")
	FName SlamWarpTargetName = FName("SlamTarget");

	/** 런칭 시 적과의 거리 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MotionWarping", meta = (ClampMin = "50.0", ClampMax = "200.0"))
	float LaunchApproachDistance = 100.0f;
	/** 슬램 시 적 위 오프셋 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MotionWarping", meta = (ClampMin = "50.0", ClampMax = "300.0"))
	float SlamHeightOffset = 150.0f;
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

USTRUCT(BlueprintType)
struct FKRAerialComboPhysicsSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float AirborneGravityScale = 0.3f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics", meta = (ClampMin = "1.0", ClampMax = "5.0"))
	float SlamGravityScale = 3.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float EnemyAirborneGravityScale = 0.1f;
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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Launch")
	FKRAerialComboLaunchSettings Launch;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slam")
	FKRAerialComboSlamSettings Slam;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	FKRAerialComboDamageSettings Damage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	FKRAerialComboMontageSettings Montage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	FKRAerialComboEffectSettings Effects;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	FKRAerialComboSoundSettings Sound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MotionWarping")
	FKRAerialComboMotionWarpingSettings MotionWarping;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting")
	FKRAerialComboTargetingSettings Targeting;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	FKRAerialComboCameraSettings Camera;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Physics")
	FKRAerialComboPhysicsSettings Physics;
	
	UFUNCTION(BlueprintPure, Category = "AerialCombo")
	float GetEstimatedDuration() const
	{
		// Launch + Airborne + Slam + Landing 대략적 시간
		return 2.0f + Launch.AirborneHangTime + Slam.HitStopDuration;
	}
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("AerialComboData"), GetFName());
	}
};
