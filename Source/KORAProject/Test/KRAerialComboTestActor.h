#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Data/DataAssets/KRAerialComboData.h"
#include "KRAerialComboTestActor.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class ACharacter;
class UAnimMontage;
class UKRAerialComboData;
class USkeletalMeshComponent;
class UCapsuleComponent;

/**
 * Aerial Combo 스킬 페이즈 (테스트용)
 */
UENUM(BlueprintType)
enum class ETestAerialComboPhase : uint8
{
	None,
	Launch,         // 적을 위로 띄우기
	Rising,         // 플레이어 상승
	Airborne,       // 공중 추격
	Slam,           // 내리찍기
	Landing,        // 착지
	Recovery        // 스킬 종료
};

/**
 * Aerial Combo 스킬 테스트용 Actor (타겟 역할)
 *
 * 이 Actor는 타겟(적) 역할을 하며, 플레이어 캐릭터가 이 Actor를 향해
 * 런칭 → 공중 추격 → 슬램 공격을 수행
 *
 * 사용법:
 * 1. BP_AerialComboTestActor 블루프린트 생성
 * 2. 에셋 할당: Montages, Effects, Sounds 등
 * 3. 레벨에 배치
 * 4. Play → 플레이어가 근처에서 키 입력으로 스킬 발동
 */
UCLASS(Blueprintable)
class KORAPROJECT_API AKRAerialComboTestActor : public AActor
{
	GENERATED_BODY()

public:
	AKRAerialComboTestActor();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// ─────────────────────────────────────────────────────
	// 컴포넌트
	// ─────────────────────────────────────────────────────

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> MeshComponent;

	// ─────────────────────────────────────────────────────
	// 데이터 에셋
	// ─────────────────────────────────────────────────────

	/** Aerial Combo 데이터 에셋 (설정하면 아래 개별 설정 무시) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AerialCombo|Data")
	TObjectPtr<UKRAerialComboData> AerialComboData;

	/** 데이터 에셋 사용 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AerialCombo|Data")
	bool bUseDataAsset = false;

	// ─────────────────────────────────────────────────────
	// 런칭 설정
	// ─────────────────────────────────────────────────────

	/** 적 런칭 높이 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AerialCombo|Launch", meta = (ClampMin = "100.0", ClampMax = "1000.0"))
	float EnemyLaunchHeight = 500.0f;

	/** 적 런칭 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AerialCombo|Launch", meta = (ClampMin = "500.0", ClampMax = "2000.0"))
	float EnemyLaunchVelocity = 1200.0f;

	/** 플레이어 점프 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AerialCombo|Launch", meta = (ClampMin = "500.0", ClampMax = "2000.0"))
	float PlayerJumpVelocity = 1000.0f;

	/** 런칭 후 공중 체류 시간 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AerialCombo|Launch", meta = (ClampMin = "0.1", ClampMax = "1.0"))
	float AirborneHangTime = 0.3f;

	// ─────────────────────────────────────────────────────
	// 슬램 설정
	// ─────────────────────────────────────────────────────

	/** 내리찍기 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AerialCombo|Slam", meta = (ClampMin = "1000.0", ClampMax = "5000.0"))
	float SlamVelocity = 2000.0f;

	/** 착지 충격파 반경 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AerialCombo|Slam", meta = (ClampMin = "100.0", ClampMax = "500.0"))
	float ImpactRadius = 300.0f;

	/** 슬램 히트스톱 시간 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AerialCombo|Slam", meta = (ClampMin = "0.0", ClampMax = "0.5"))
	float SlamHitStopDuration = 0.15f;

	// ─────────────────────────────────────────────────────
	// 몽타주
	// ─────────────────────────────────────────────────────

	/** 런칭 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AerialCombo|Montage")
	TObjectPtr<UAnimMontage> LaunchMontage;

	/** 상승 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AerialCombo|Montage")
	TObjectPtr<UAnimMontage> RisingMontage;

	/** 공중 대기 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AerialCombo|Montage")
	TObjectPtr<UAnimMontage> AirborneMontage;

	/** 슬램 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AerialCombo|Montage")
	TObjectPtr<UAnimMontage> SlamMontage;

	/** 착지 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AerialCombo|Montage")
	TObjectPtr<UAnimMontage> LandingMontage;

	/** 몽타주 재생 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AerialCombo|Montage", meta = (ClampMin = "0.5", ClampMax = "2.0"))
	float MontagePlayRate = 1.0f;

	// ─────────────────────────────────────────────────────
	// 이펙트 태그 (개별 설정 - bUseDataAsset = false 일 때)
	// ─────────────────────────────────────────────────────

	/** 런칭 상승 기류 이펙트 태그 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AerialCombo|Effect", meta = (Categories = "Effect.AerialCombo"))
	FGameplayTag LaunchWindEffectTag;

	/** 플레이어 잔상 이펙트 태그 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AerialCombo|Effect", meta = (Categories = "Effect.AerialCombo"))
	FGameplayTag AfterImageEffectTag;

	/** 슬램 궤적 이펙트 태그 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AerialCombo|Effect", meta = (Categories = "Effect.AerialCombo"))
	FGameplayTag SlamTrailEffectTag;

	/** 착지 충격파 이펙트 태그 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AerialCombo|Effect", meta = (Categories = "Effect.AerialCombo"))
	FGameplayTag ImpactEffectTag;

	/** 바닥 균열 이펙트 태그 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AerialCombo|Effect", meta = (Categories = "Effect.AerialCombo"))
	FGameplayTag GroundCrackEffectTag;

	/** 이펙트 색상 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AerialCombo|Effect")
	FLinearColor EffectColor = FLinearColor(0.8f, 0.2f, 0.1f, 1.0f);

	// ─────────────────────────────────────────────────────
	// 물리 설정
	// ─────────────────────────────────────────────────────

	/** 공중 중력 스케일 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AerialCombo|Physics", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float AirborneGravityScale = 0.3f;

	/** 슬램 중력 스케일 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AerialCombo|Physics", meta = (ClampMin = "1.0", ClampMax = "5.0"))
	float SlamGravityScale = 3.0f;

	// ─────────────────────────────────────────────────────
	// 테스트 옵션
	// ─────────────────────────────────────────────────────

	/** 시작 시 자동 실행 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AerialCombo|Test")
	bool bAutoStartOnBeginPlay = false;

	/** 자동 실행 딜레이 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AerialCombo|Test", meta = (EditCondition = "bAutoStartOnBeginPlay"))
	float AutoStartDelay = 1.0f;

	/** 디버그: 페이즈 정보 표시 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AerialCombo|Debug")
	bool bDrawDebugInfo = true;

	/** 디버그: 타겟 위치 표시 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AerialCombo|Debug")
	bool bDrawDebugTarget = true;

	// ─────────────────────────────────────────────────────
	// 함수
	// ─────────────────────────────────────────────────────

	/** Aerial Combo 스킬 시작 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "AerialCombo")
	void StartAerialCombo();

	/** Aerial Combo 스킬 중지 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "AerialCombo")
	void StopAerialCombo();

	/** 현재 스킬 페이즈 가져오기 */
	UFUNCTION(BlueprintPure, Category = "AerialCombo")
	ETestAerialComboPhase GetCurrentPhase() const { return CurrentPhase; }

	/** 타겟(이 Actor) 위치 가져오기 */
	UFUNCTION(BlueprintPure, Category = "AerialCombo")
	FVector GetTargetActorLocation() const { return GetActorLocation(); }

protected:
	// ─────────────────────────────────────────────────────
	// 데이터 에셋 헬퍼
	// ─────────────────────────────────────────────────────

	float GetEnemyLaunchVelocity() const;
	float GetPlayerJumpVelocity() const;
	float GetAirborneHangTime() const;
	float GetSlamVelocity() const;
	float GetImpactRadius() const;
	float GetSlamHitStopDuration() const;
	float GetAirborneGravityScale() const;
	float GetSlamGravityScale() const;
	float GetMontagePlayRate() const;

	UAnimMontage* GetLaunchMontage() const;
	UAnimMontage* GetRisingMontage() const;
	UAnimMontage* GetAirborneMontage() const;
	UAnimMontage* GetSlamMontage() const;
	UAnimMontage* GetLandingMontage() const;

	FGameplayTag GetLaunchWindEffectTag() const;
	FGameplayTag GetAfterImageEffectTag() const;
	FGameplayTag GetSlamTrailEffectTag() const;
	FGameplayTag GetImpactEffectTag() const;
	FGameplayTag GetGroundCrackEffectTag() const;
	FLinearColor GetEffectColor() const;

	// ─────────────────────────────────────────────────────
	// 페이즈 실행 함수
	// ─────────────────────────────────────────────────────

	/** 런칭 페이즈 실행 */
	void ExecuteLaunchPhase();

	/** 상승 페이즈 실행 */
	void ExecuteRisingPhase();

	/** 공중 페이즈 실행 */
	void ExecuteAirbornePhase();

	/** 슬램 페이즈 실행 */
	void ExecuteSlamPhase();

	/** 착지 페이즈 실행 */
	void ExecuteLandingPhase();

	/** 스킬 완료 */
	void CompleteAerialCombo();

	// ─────────────────────────────────────────────────────
	// 캐릭터 제어
	// ─────────────────────────────────────────────────────

	/** 플레이어 캐릭터 찾기 */
	ACharacter* GetPlayerCharacter() const;

	/** 플레이어 점프 */
	void LaunchPlayer();

	/** 중력 스케일 설정 */
	void SetPlayerGravityScale(float Scale);

	/** 원래 중력 스케일 복구 */
	void RestorePlayerGravityScale();

	/** 착지 체크 */
	void CheckLanding();

	// ─────────────────────────────────────────────────────
	// 몽타주 제어
	// ─────────────────────────────────────────────────────

	/** 몽타주 재생 */
	void PlayMontage(UAnimMontage* Montage, float PlayRate = 1.0f);

	/** 현재 몽타주 중지 */
	void StopCurrentMontage();

	// ─────────────────────────────────────────────────────
	// 이펙트 제어
	// ─────────────────────────────────────────────────────

	/** 런칭 이펙트 스폰 */
	void SpawnLaunchEffect();

	/** 잔상 이펙트 시작 */
	void StartAfterImageEffect();

	/** 잔상 이펙트 중지 */
	void StopAfterImageEffect();

	/** 슬램 궤적 이펙트 스폰 */
	void SpawnSlamTrailEffect();

	/** 착지 충격파 이펙트 스폰 */
	void SpawnImpactEffect();

	/** 바닥 균열 이펙트 스폰 */
	void SpawnGroundCrackEffect();

	/** 히트스톱 적용 */
	void ApplyHitStop(float Duration);

	// ─────────────────────────────────────────────────────
	// 디버그
	// ─────────────────────────────────────────────────────

	/** 디버그 정보 그리기 */
	void DrawDebug();

private:
	/** 현재 스킬 페이즈 */
	ETestAerialComboPhase CurrentPhase = ETestAerialComboPhase::None;

	/** 원래 중력 스케일 */
	float OriginalGravityScale = 1.0f;

	/** 스킬 시작 위치 */
	FVector SkillStartLocation;

	/** 타겟(이 Actor)의 시작 위치 */
	FVector TargetStartLocation;

	/** 스폰된 잔상 이펙트 */
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ActiveAfterImageEffect;

	/** 스폰된 슬램 궤적 이펙트 */
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ActiveSlamTrailEffect;

	/** 타이머 핸들 */
	FTimerHandle PhaseTimerHandle;

	/** 착지 체크 타이머 핸들 */
	FTimerHandle LandingCheckTimerHandle;

	/** 자동 시작 타이머 핸들 */
	FTimerHandle AutoStartTimerHandle;

	/** 슬램 중 플레이어 위치 보간 */
	bool bIsSlamming = false;
	FVector SlamStartLocation;
	float SlamStartTime = 0.0f;
};
