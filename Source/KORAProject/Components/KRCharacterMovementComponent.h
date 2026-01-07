#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KRCharacterMovementComponent.generated.h"

class UKRAbilitySystemComponent;
class AKRLadderActor;

UENUM(BlueprintType)
enum class EKRMovementMode : uint8
{
	Ground = 0,
	Ladder,
};

UCLASS()
class KORAPROJECT_API UKRCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	virtual float GetMaxSpeed() const override;

	// ─────────────────────────────────────────────────────
	// Ladder System
	// ─────────────────────────────────────────────────────

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StartClimbingLadder(const AKRLadderActor* NewLadder);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StopClimbingLadder();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetLadderMounting(bool bMounting);

	UFUNCTION(BlueprintPure, Category = "Movement")
	float GetLadderClimbDirection() const { return GetClimbDirection(); }

	const AKRLadderActor* GetCurrentLadder() const { return CurrentLadder; }

	// ─────────────────────────────────────────────────────
	// Character Separation System
	// ─────────────────────────────────────────────────────

	/** 캐릭터 분리 시스템 활성화/비활성화 */
	UFUNCTION(BlueprintCallable, Category = "Movement|Separation")
	void SetCharacterSeparationEnabled(bool bEnabled);

	/** 캐릭터 분리 시스템 활성화 여부 */
	UFUNCTION(BlueprintPure, Category = "Movement|Separation")
	bool IsCharacterSeparationEnabled() const { return bEnableCharacterSeparation; }

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	
	void PhysLadder(float deltaTime, int32 Iterations);

	float GetClimbDirection() const;
	
	void SnapToLadderLocation();
	
	void CheckLadderExit(float ClimbDirection);

	bool bIsLadderMounting = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ladder")
	float MaxLadderSpeed = 150.f;
	
	UPROPERTY(Transient)
	mutable TWeakObjectPtr<UKRAbilitySystemComponent> CachedASC;

	UPROPERTY(Transient)
	TObjectPtr<const AKRLadderActor> CurrentLadder;

	// ─────────────────────────────────────────────────────
	// Character Separation Properties
	// ─────────────────────────────────────────────────────

	/**
	 * 캐릭터 분리 시스템 활성화 여부
	 * 다른 캐릭터와 겹칠 때 부드럽게 밀어내는 기능
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Separation")
	bool bEnableCharacterSeparation = true;

	/**
	 * 분리력 강도 (cm/s)
	 * 다른 캐릭터와 겹칠 때 밀어내는 속도
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Separation", meta = (ClampMin = "100.0", ClampMax = "2000.0", EditCondition = "bEnableCharacterSeparation"))
	float SeparationStrength = 600.0f;

	/**
	 * 분리 감지 추가 반경 (캡슐 반경에 더함)
	 * 0 = 캡슐이 완전히 겹쳐야 분리 시작
	 * 값이 클수록 더 일찍 분리 시작
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Separation", meta = (ClampMin = "0.0", ClampMax = "100.0", EditCondition = "bEnableCharacterSeparation"))
	float SeparationDetectionRadius = 20.0f;

	/**
	 * 최소 분리 거리 (캡슐 반경 합계에 추가)
	 * 캐릭터 간 유지해야 할 최소 거리 버퍼
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Separation", meta = (ClampMin = "0.0", ClampMax = "100.0", EditCondition = "bEnableCharacterSeparation"))
	float MinSeparationBuffer = 10.0f;

	/**
	 * 공격 중 분리력 배율
	 * 공격 상태에서는 분리력을 줄여 타겟에 더 가깝게 유지
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Movement: Separation", meta = (ClampMin = "0.0", ClampMax = "1.0", EditCondition = "bEnableCharacterSeparation"))
	float SeparationMultiplierWhileAttacking = 0.3f;

private:
	/** 캐릭터 분리력 적용 */
	void ApplyCharacterSeparation(float DeltaTime);

	/** 현재 공격 중인지 확인 (분리력 감소용) */
	bool IsPerformingAttack() const;
};