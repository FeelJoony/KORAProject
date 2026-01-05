#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "Data/KRMeleeAttackTypes.h"
#include "KRGA_CoreDriveBurst_Sword.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UKRCoreDriveComponent;
class UGameplayEffect;
class UKRCameraMode;

/**
 * 코어드라이브 버스트 공격 설정
 */
USTRUCT(BlueprintType)
struct FKRCoreDriveBurstAttackConfig
{
	GENERATED_BODY()

	/** 공격 범위 형태 */
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	EKRAttackShapeType AttackShape = EKRAttackShapeType::Sphere;

	/** 스피어 반경 */
	UPROPERTY(EditDefaultsOnly, Category = "Attack",
		meta = (EditCondition = "AttackShape == EKRAttackShapeType::Sphere", EditConditionHides))
	float SphereRadius = 300.0f;

	/** 박스 크기 */
	UPROPERTY(EditDefaultsOnly, Category = "Attack",
		meta = (EditCondition = "AttackShape == EKRAttackShapeType::Box", EditConditionHides))
	FVector BoxExtent = FVector(200.0f, 200.0f, 200.0f);

	/** 공격 시작 오프셋 (캐릭터 기준 전방) */
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float ForwardOffset = 150.0f;

	/** 총 데미지 횟수 */
	UPROPERTY(EditDefaultsOnly, Category = "Attack", meta = (ClampMin = "1", ClampMax = "20"))
	int32 HitCount = 8;

	/** 히트 간격 (초) */
	UPROPERTY(EditDefaultsOnly, Category = "Attack", meta = (ClampMin = "0.05", ClampMax = "1.0"))
	float HitInterval = 0.15f;

	/** 마지막 히트 전 추가 딜레이 (초) - 애니메이션과 동기화용 */
	UPROPERTY(EditDefaultsOnly, Category = "Attack", meta = (ClampMin = "0.0", ClampMax = "3.0"))
	float FinalHitDelay = 0.5f;

	/** 히트당 데미지 배율 */
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float DamageMultiplierPerHit = 0.8f;

	/** 마지막 히트 데미지 배율 (피니셔) */
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float FinalHitDamageMultiplier = 2.0f;
};

/**
 * 버스트 페이즈
 */
UENUM(BlueprintType)
enum class EKRCoreDriveBurstPhase : uint8
{
	None,
	Startup,    // 준비 자세 - 하이퍼아머
	Attack,     // 공격 + 종료 - 무적 + 다단히트
	Finished
};

/**
 * 코어드라이브 버스트 (Sword) - 공중 난무 공격
 *
 * - 코어드라이브 게이지 전체 소모 (가득 찼을 때만 사용 가능)
 * - 2페이즈 구성: Startup(하이퍼아머) → Attack(무적+다단히트+종료)
 * - 타이머 기반 다단 히트
 */
UCLASS()
class KORAPROJECT_API UKRGA_CoreDriveBurst_Sword : public UKRGameplayAbility
{
	GENERATED_BODY()

public:
	UKRGA_CoreDriveBurst_Sword(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	// ─────────────────────────────────────────
	// 몽타주 설정
	// ─────────────────────────────────────────

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Animation")
	TObjectPtr<UAnimMontage> StartupMontage;

	/** 공격 + 종료 몽타주 (합쳐진 애니메이션) */
	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Animation")
	TObjectPtr<UAnimMontage> AttackMontage;

	/** 루트 모션 사용 여부 - true면 애니메이션의 루트 모션에 따라 캐릭터가 이동 */
	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Animation")
	bool bUseRootMotion = true;

	// ─────────────────────────────────────────
	// 공격 설정
	// ─────────────────────────────────────────

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Attack")
	FKRCoreDriveBurstAttackConfig AttackConfig;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	// ─────────────────────────────────────────
	// GameplayCue 설정
	// ─────────────────────────────────────────

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Cue")
	FGameplayTag StartupCueTag;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Cue")
	FGameplayTag AttackCueTag;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Cue")
	FGameplayTag HitCueTag;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Cue")
	FGameplayTag FinishCueTag;

	// ─────────────────────────────────────────
	// 카메라 모드 설정
	// ─────────────────────────────────────────

	/** 버스트 스킬용 카메라 모드 클래스 */
	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Camera")
	TSubclassOf<UKRCameraMode> BurstCameraModeClass;

	/** 카메라 모드 활성화 여부 */
	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Camera")
	bool bUseCameraMode = true;

	// ─────────────────────────────────────────
	// 디버그 설정
	// ─────────────────────────────────────────

	/** 디버그 시각화 활성화 */
	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Debug")
	bool bShowDebugShape = false;

	/** 디버그 도형 지속 시간 */
	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Debug", meta = (EditCondition = "bShowDebugShape"))
	float DebugDrawDuration = 0.5f;

private:
	// ─────────────────────────────────────────
	// 상태 관리
	// ─────────────────────────────────────────

	EKRCoreDriveBurstPhase CurrentPhase = EKRCoreDriveBurstPhase::None;

	void TransitionToPhase(EKRCoreDriveBurstPhase NewPhase);
	void StartStartupPhase();
	void StartAttackPhase();

	// ─────────────────────────────────────────
	// 상태 태그
	// ─────────────────────────────────────────

	void ApplyHyperArmor();       // 히트리액션 면역만
	void RemoveHyperArmor();
	void ApplyFullInvincibility(); // 완전 무적
	void RemoveFullInvincibility();

	// ─────────────────────────────────────────
	// 다단 히트
	// ─────────────────────────────────────────

	void StartMultiHitSequence();
	void StopMultiHitSequence();
	void PerformMultiHit();
	void ScheduleFinalHit();
	void PerformFinalHit();

	FTimerHandle MultiHitTimerHandle;
	FTimerHandle FinalHitTimerHandle;
	int32 CurrentHitIndex = 0;

	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitActorsThisHit;

	// ─────────────────────────────────────────
	// 판정 고정
	// ─────────────────────────────────────────
	FVector BurstAnchorLocation;
	FRotator BurstAnchorRotation;

	// ─────────────────────────────────────────
	// 유틸리티
	// ─────────────────────────────────────────

	UKRCoreDriveComponent* GetCoreDriveComponent() const;
	void ApplyDamageToTarget(AActor* TargetActor, float DamageMultiplier);
	void ExecuteCue(const FGameplayTag& CueTag);
	void ExecuteCueAtLocation(const FGameplayTag& CueTag, const FVector& Location);

	void PlayMontageWithCallback(UAnimMontage* Montage, void(UKRGA_CoreDriveBurst_Sword::*Callback)());
	
	// ─────────────────────────────────────────
	// 콜리전 제어
	// ─────────────────────────────────────────
	void SetIgnorePawnCollision(bool bIgnore);
	TMap<ECollisionChannel, ECollisionResponse> PreviousCollisionResponses;

	
	// ─────────────────────────────────────────
	// 이동 제어
	// ─────────────────────────────────────────

	void DisableMovement();
	void EnableMovement();

	// ─────────────────────────────────────────
	// 카메라 모드
	// ─────────────────────────────────────────

	void PushCameraMode();
	void RemoveCameraMode();

	// ─────────────────────────────────────────
	// 디버그 시각화
	// ─────────────────────────────────────────

	void DrawDebugAttackShape();
	
	// ─────────────────────────────────────────
	// 몽타주 콜백
	// ─────────────────────────────────────────

	UFUNCTION()
	void OnStartupMontageCompleted();

	UFUNCTION()
	void OnAttackMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

protected:
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;
};
