#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "KRGA_CoreDriveSurge_Sword.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;
class UKRCoreDriveComponent;
class UGameplayEffect;
class UKRStarDashData;

/**
 * 코어드라이브 서지 (Sword) - 전방 돌진 공격
 *
 * - 코어드라이브 게이지 1칸(25) 소모
 * - 몽타주 재생 중 무적 (KRTAG_STATE_IMMUNE_DAMAGE)
 * - 대시 중 적 통과 가능 (Collision Overlap)
 * - 시작~도착 지점 사이 적에게 데미지 (Box Sweep)
 * - Motion Warping으로 대시 거리 조절
 */ 
UCLASS()
class KORAPROJECT_API UKRGA_CoreDriveSurge_Sword : public UKRGameplayAbility
{
	GENERATED_BODY()

public:
	UKRGA_CoreDriveSurge_Sword(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	/**
	 * 대시 데미지 처리 (GameplayEvent 기반)
	 * AnimNotifyState가 GameplayEvent를 발송하면 WaitGameplayEvent Task가 수신하여 호출
	 */
	UFUNCTION(BlueprintCallable, Category = "CoreDrive")
	void PerformDashDamageFromNotify();

protected:
	// ─────────────────────────────────────────
	// 애니메이션 설정
	// ─────────────────────────────────────────

	/** 대시 몽타주 */
	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Animation")
	TObjectPtr<UAnimMontage> DashMontage;

	// ─────────────────────────────────────────
	// 코스트 설정
	// ─────────────────────────────────────────

	/** 코어드라이브 소모량 (1칸 = 25.0f 기준) */
	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Cost")
	float CoreDriveCost = 25.0f;

	// ─────────────────────────────────────────
	// 데미지 설정
	// ─────────────────────────────────────────

	/** 데미지 배율 */
	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Damage")
	float DamageMultiplier = 2.5f;

	/** 데미지 GE 클래스 */
	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	// ─────────────────────────────────────────
	// 대시 설정
	// ─────────────────────────────────────────

	/** 대시 거리 */
	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Dash", meta = (ClampMin = "200.0", ClampMax = "2000.0"))
	float DashDistance = 800.0f;

	/** 박스 트레이스 하프 익스텐트 */
	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Dash")
	FVector BoxHalfExtent = FVector(50.0f, 100.0f, 100.0f);

	/** 모션 워핑 타겟 이름 */
	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Dash")
	FName WarpTargetName = FName("SurgeDash");

	// ─────────────────────────────────────────
	// 디버그 설정
	// ─────────────────────────────────────────

	/** 디버그 시각화 활성화 */
	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Debug")
	bool bShowDebugShape = false;

	/** 디버그 도형 지속 시간 */
	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Debug", meta = (EditCondition = "bShowDebugShape"))
	float DebugDrawDuration = 2.0f;

	// ─────────────────────────────────────────
	// GameplayCue 설정
	// ─────────────────────────────────────────

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Cue")
	FGameplayTag DashStartCueTag;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Cue")
	FGameplayTag DashHitCueTag;

	// ─────────────────────────────────────────
	// Star 이펙트 설정 (KRStarDashData 사용)
	// ─────────────────────────────────────────

	/** Star Dash 데이터 에셋 (대시 궤적 이펙트 설정) */
	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|StarEffect")
	TObjectPtr<UKRStarDashData> StarDashData;

	// ─────────────────────────────────────────
	// GameplayEvent 설정
	// ─────────────────────────────────────────

	/** 히트 체크 GameplayEvent 태그 (AnimNotifyState에서 발송) */
	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Event")
	FGameplayTag HitCheckEventTag;

private:
	// ─────────────────────────────────────────
	// 내부 함수
	// ─────────────────────────────────────────

	/** GameplayEvent 리스너 설정 */
	void SetupHitCheckEventListener();

	/** 히트 체크 이벤트 수신 콜백 */
	UFUNCTION()
	void OnHitCheckEventReceived(FGameplayEventData Payload);

	/** 코어드라이브 컴포넌트 가져오기 */
	UKRCoreDriveComponent* GetCoreDriveComponent() const;

	/** 무적 태그 적용 */
	void ApplyInvincibility();

	/** 무적 태그 제거 */
	void RemoveInvincibility();

	/** 콜리전 응답 변경 (적 통과용) */
	void SetCollisionIgnorePawns(bool bIgnore);

	/** 대시 경로 데미지 처리 */
	void PerformDashDamage();

	/** 모션 워핑 설정 */
	void SetupDashWarping();

	/** 타겟에게 데미지 적용 */
	void ApplyDamageToTarget(AActor* TargetActor, const FHitResult& HitResult);

	/** 대시 시작 Cue 실행 */
	void ExecuteDashStartCue();

	/** 히트 Cue 실행 */
	void ExecuteHitCue(const FVector& HitLocation);

	/** 대시 궤적 이펙트 스폰 (Star Effect) */
	void SpawnDashTrailEffect();

	/** 이동 비활성화 (시전 중 이동 불가) */
	void DisableMovement();

	/** 이동 활성화 (종료 시 복원) */
	void EnableMovement();

	/** 디버그 도형 그리기 */
	void DrawDebugHitBox();

	// ─────────────────────────────────────────
	// 몽타주 콜백
	// ─────────────────────────────────────────

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

protected:
	// ─────────────────────────────────────────
	// 런타임 상태
	// ─────────────────────────────────────────

	FVector DashStartLocation;
	FVector DashEndLocation;

	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitActors;

	/** 원래 콜리전 응답 (복원용) */
	TEnumAsByte<ECollisionResponse> OriginalPawnResponse = ECR_Block;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> HitCheckEventTask;

	/** 원래 중력 스케일 (복원용) */
	float OriginalGravityScale = 1.0f;

	/** 원래 이동 모드 (복원용) */
	TEnumAsByte<EMovementMode> OriginalMovementMode = MOVE_Walking;
};
