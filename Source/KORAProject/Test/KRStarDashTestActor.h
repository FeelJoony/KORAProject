#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "GAS/Abilities/HeroAbilities/Libraries/KRStarPatternLibrary.h"
#include "Data/DataAssets/KRStarDashData.h"
#include "KRStarDashTestActor.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class ACharacter;
class UAnimMontage;
class UMaterialInstanceDynamic;
class UKRStarDashData;

/**
 * Star Dash 스킬 테스트용 Actor (타겟 역할)
 *
 * 이 Actor는 타겟(적) 역할을 하며, 플레이어 캐릭터가 이 주변을 별 모양으로 이동하면서 공격
 *
 * 사용법:
 * 1. BP_StarDashTestActor 블루프린트 생성
 * 2. 에셋 할당: StarDashNiagara, AfterImageEffect, DashMontage
 * 3. 레벨에 배치
 * 4. Play → 플레이어가 근처에서 키 입력으로 스킬 발동
 */
UCLASS(Blueprintable)
class KORAPROJECT_API AKRStarDashTestActor : public AActor
{
	GENERATED_BODY()

public:
	AKRStarDashTestActor();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// ─────────────────────────────────────────────────────
	// 데이터 에셋
	// ─────────────────────────────────────────────────────

	/** Star Dash 데이터 에셋 (설정하면 아래 개별 설정 무시) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Data")
	TObjectPtr<UKRStarDashData> StarDashData;

	/** 데이터 에셋 사용 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Data")
	bool bUseDataAsset = false;

	// ─────────────────────────────────────────────────────
	// 이펙트 태그 (개별 설정 - bUseDataAsset = false 일 때)
	// ─────────────────────────────────────────────────────

	/** Star Dash 라인 이펙트 태그 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Effect", meta = (Categories = "Effect.StarDash"))
	FGameplayTag SlashLineEffectTag;

	/** 잔상 이펙트 태그 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Effect", meta = (Categories = "Effect.StarDash"))
	FGameplayTag AfterImageEffectTag;

	/** SplineVFX Lightning 블루프린트 클래스 (BP_SplineVFX_ElectricLightning) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Effect")
	TSubclassOf<AActor> SplineLightningClass;

	/** Lightning 이펙트 활성화 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Effect")
	bool bEnableLightning = true;

	// ─────────────────────────────────────────────────────
	// 사운드 태그 (개별 설정 - bUseDataAsset = false 일 때)
	// ─────────────────────────────────────────────────────

	/** 대시 시작 사운드 태그 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Sound", meta = (Categories = "Sound.StarDash"))
	FGameplayTag DashStartSoundTag;

	/** 슬래시 사운드 태그 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Sound", meta = (Categories = "Sound.StarDash"))
	FGameplayTag SlashSoundTag;

	/** 피니시 사운드 태그 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Sound", meta = (Categories = "Sound.StarDash"))
	FGameplayTag FinishSoundTag;

	// ─────────────────────────────────────────────────────
	// 몽타주
	// ─────────────────────────────────────────────────────

	/** 대시 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Montage")
	TObjectPtr<UAnimMontage> DashMontage;

	/** 몽타주 재생 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Montage")
	float MontagePlayRate = 2.0f;

	// ─────────────────────────────────────────────────────
	// 별 패턴 설정
	// ─────────────────────────────────────────────────────

	/** 별 반지름 (외곽) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Pattern", meta = (ClampMin = "50.0", ClampMax = "1000.0"))
	float Radius = 300.0f;

	/** 내부 반지름 (10포인트 모드용, 0이면 자동 계산) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Pattern", meta = (ClampMin = "0.0", ClampMax = "500.0"))
	float InnerRadius = 0.0f;

	/** 높이 오프셋 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Pattern")
	float HeightOffset = 0.0f;

	/**
	 * 10포인트 별 패턴 사용 (외곽 5개 + 내부 5개)
	 * true: 별 윤곽선 그리기 (10개 라인)
	 * false: 기존 별 그리기 (5개 라인, 꼭짓점 스킵)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Pattern")
	bool bUse10PointStar = true;

	// ─────────────────────────────────────────────────────
	// 타이밍 설정
	// ─────────────────────────────────────────────────────

	/** 각 대시 시간 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Timing", meta = (ClampMin = "0.05", ClampMax = "1.0"))
	float DashDuration = 0.15f;

	/** 대시 사이 딜레이 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Timing", meta = (ClampMin = "0.0", ClampMax = "0.5"))
	float DashInterval = 0.05f;

	/** 라인 이펙트 그리기 시간 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Timing")
	float LineDrawDuration = 1.0f;

	// ─────────────────────────────────────────────────────
	// 투명도 설정
	// ─────────────────────────────────────────────────────

	/** 대시 중 투명도 (0 = 완전 투명, 1 = 불투명) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Visibility", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DashOpacity = 0.3f;

	/** 머티리얼 Opacity 파라미터 이름 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Visibility")
	FName OpacityParameterName = FName("Opacity");

	/**
	 * Visibility 숨기기 모드 사용 여부
	 * true: 캐릭터를 완전히 숨김 (간단하지만 안 보임)
	 * false: 머티리얼 Opacity 파라미터 사용 (머티리얼이 Translucent/Masked여야 함)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Visibility")
	bool bUseVisibilityHide = false;

	/**
	 * CustomPrimitiveData 인덱스 (Dithered Opacity용)
	 * 머티리얼에서 이 인덱스의 CustomPrimitiveData를 읽어 DitherTemporalAA에 연결하면 작동
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Visibility", meta = (ClampMin = "0", ClampMax = "7"))
	int32 OpacityPrimitiveDataIndex = 0;

	/** CustomPrimitiveData로 Dithered Opacity 사용 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Visibility")
	bool bUseDitheredOpacity = true;

	// ─────────────────────────────────────────────────────
	// 이펙트 설정
	// ─────────────────────────────────────────────────────

	/** 라인 색상 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Effect")
	FLinearColor LineColor = FLinearColor(1.0f, 0.8f, 0.2f, 1.0f);

	/** 라인 두께 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Effect", meta = (ClampMin = "1.0", ClampMax = "50.0"))
	float LineWidth = 15.0f;

	/** 글로우 강도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Effect", meta = (ClampMin = "1.0", ClampMax = "20.0"))
	float GlowIntensity = 5.0f;

	/** 잔상 색상 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Effect")
	FLinearColor AfterImageColor = FLinearColor(0.5f, 0.8f, 1.0f, 0.5f);

	// ─────────────────────────────────────────────────────
	// 카메라 설정
	// ─────────────────────────────────────────────────────

	/** 스킬 중 카메라 모드 활성화 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Camera")
	bool bEnableCameraMode = true;

	/** 스킬용 카메라 모드 클래스 (데이터 에셋 설정 오버라이드) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Camera")
	TSubclassOf<class UKRCameraMode> CameraModeClass;

	/** 카메라 높이 오프셋 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Camera", meta = (ClampMin = "0.0", ClampMax = "1000.0"))
	float CameraHeightOffset = 400.0f;

	/** 카메라 뒤쪽 거리 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Camera", meta = (ClampMin = "100.0", ClampMax = "1500.0"))
	float CameraBackwardDistance = 600.0f;

	/** 카메라 LookAt 높이 오프셋 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Camera", meta = (ClampMin = "0.0", ClampMax = "500.0"))
	float CameraLookAtHeightOffset = 100.0f;

	/** 스킬 중 FOV */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Camera", meta = (ClampMin = "60.0", ClampMax = "120.0"))
	float SkillFOV = 90.0f;

	// ─────────────────────────────────────────────────────
	// 테스트 옵션
	// ─────────────────────────────────────────────────────

	/** 시작 시 자동 실행 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Test")
	bool bAutoStartOnBeginPlay = false;

	/** 자동 실행 딜레이 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Test", meta = (EditCondition = "bAutoStartOnBeginPlay"))
	float AutoStartDelay = 1.0f;

	/** 디버그: 별 꼭짓점 표시 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Debug")
	bool bDrawDebugPoints = true;

	/** 디버그: 라인 표시 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StarDash|Debug")
	bool bDrawDebugLines = true;

	// ─────────────────────────────────────────────────────
	// 함수
	// ─────────────────────────────────────────────────────

	/** Star Dash 스킬 시작 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "StarDash")
	void StartStarDash();

	/** Star Dash 스킬 중지 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "StarDash")
	void StopStarDash();

	/** 라인 이펙트만 스폰 (캐릭터 이동 없이) */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "StarDash")
	void SpawnLineEffectOnly();

	/** 현재 별 패턴 데이터 가져오기 */
	UFUNCTION(BlueprintPure, Category = "StarDash")
	FKRStarPatternData GetCurrentStarPattern() const { return CurrentPattern; }

protected:
	// ─────────────────────────────────────────────────────
	// 데이터 에셋 헬퍼
	// ─────────────────────────────────────────────────────

	/** 현재 설정 기준 대시 횟수 */
	int32 GetMaxDashCount() const;

	/** 현재 설정에서 사운드 재생 (태그 기반) */
	void PlayDashSound(FVector Location);
	void PlaySlashSound(FVector Location);
	void PlayFinishSound(FVector Location);

	/** 이펙트 태그 가져오기 */
	FGameplayTag GetSlashLineEffectTag() const;
	FGameplayTag GetAfterImageEffectTag() const;

	// ─────────────────────────────────────────────────────
	// 패턴 생성
	// ─────────────────────────────────────────────────────

	/** 별 패턴 생성 (플레이어 위치를 Point[0]으로) */
	void GeneratePatternFromPlayer();

	/** 디버그 드로우 */
	void DrawDebug();

	/** 다음 포인트로 대시 */
	void DashToNextPoint();

	/** 단일 대시 실행 */
	void ExecuteDash(int32 PointIndex);

	/** 대시 완료 처리 */
	void OnDashComplete();

	/** 스킬 완료 처리 */
	void CompleteStarDash();

	// ─────────────────────────────────────────────────────
	// 캐릭터 제어
	// ─────────────────────────────────────────────────────

	/** 플레이어 캐릭터 찾기 */
	ACharacter* GetPlayerCharacter() const;

	/** 캐릭터 투명도 설정 */
	void SetCharacterOpacity(float Opacity);

	/** 캐릭터 투명도 복구 */
	void RestoreCharacterOpacity();

	/** 다이나믹 머티리얼 캐시 */
	void CacheCharacterMaterials();

	/** 잔상 이펙트 시작 */
	void StartAfterImageEffect();

	/** 잔상 이펙트 중지 */
	void StopAfterImageEffect();

	/** 대시 몽타주 시작 (스킬 시작 시 한 번만 호출, 루핑) */
	void StartDashMontage();

	/** 대시 몽타주 중지 (스킬 종료 시 호출) */
	void StopDashMontage();

	/** 복귀 몽타주 재생 (스킬 완료 시 호출) */
	void PlayFinishMontage();

	/** 라인 이펙트 스폰 */
	void SpawnLineEffect();

	/** Lightning 이펙트 스폰 (각 대시마다) */
	void SpawnLightningForDash(FVector StartPoint, FVector EndPoint);

	// ─────────────────────────────────────────────────────
	// 카메라 제어
	// ─────────────────────────────────────────────────────

	/** 카메라 모드 활성화 */
	void ActivateCameraMode();

	/** 카메라 모드 비활성화 */
	void DeactivateCameraMode();

	/** 현재 카메라 모드 클래스 가져오기 */
	TSubclassOf<class UKRCameraMode> GetCameraModeClass() const;

private:
	/** 현재 별 패턴 데이터 */
	FKRStarPatternData CurrentPattern;

	/** 현재 대시 인덱스 */
	int32 CurrentDashIndex = 0;

	/** 대시 중 여부 */
	bool bIsDashing = false;

	/** 대시 시작 위치 */
	FVector DashStartLocation;

	/** 대시 시작 시간 */
	float DashStartTime = 0.0f;

	/** 현재 스폰된 라인 이펙트 */
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> CurrentLineEffect;

	/** 스폰된 Lightning 액터들 (SplineVFX) */
	UPROPERTY()
	TArray<TObjectPtr<AActor>> SpawnedLightningActors;

	/** 활성화된 잔상 이펙트 */
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ActiveAfterImageEffect;

	/** 캐시된 다이나믹 머티리얼 */
	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstanceDynamic>> CachedMaterials;

	/** 원래 머티리얼들 */
	UPROPERTY()
	TArray<TObjectPtr<UMaterialInterface>> OriginalMaterials;

	/** 대시 타이머 핸들 */
	FTimerHandle DashTimerHandle;

	/** 자동 시작 타이머 핸들 */
	FTimerHandle AutoStartTimerHandle;

	/** 플레이어 스킬 시작 위치 (카메라용) */
	FVector PlayerSkillStartLocation = FVector::ZeroVector;

	/** 현재 활성화된 카메라 모드 클래스 */
	TSubclassOf<class UKRCameraMode> ActiveCameraModeClass;
};
