#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/KRStaminaTypes.h"
#include "KRStaminaComponent.generated.h"

class UAbilitySystemComponent;
class UKRPlayerAttributeSet;

/**
 * 스태미나 관리 컴포넌트
 * - 스태미나 소모/회복 중앙 관리
 * - 상태별 회복률 조절
 * - 지속 소모 (달리기) 지원
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class KORAPROJECT_API UKRStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UKRStaminaComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ─────────────────────────────────────────
	// 소모 API
	// ─────────────────────────────────────────
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	bool ConsumeStamina(float Amount);
	
	UFUNCTION(BlueprintPure, Category = "Stamina")
	bool HasEnoughStamina(float Amount) const;
	
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void StartContinuousConsumption(float RatePerSecond);
	
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void StopContinuousConsumption();
	
	UFUNCTION(BlueprintPure, Category = "Stamina")
	bool IsContinuousConsuming() const { return bIsContinuousConsuming; }

	// ─────────────────────────────────────────
	// 회복 API
	// ─────────────────────────────────────────
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void SetRecoveryState(EStaminaRecoveryState NewState);
	
	UFUNCTION(BlueprintPure, Category = "Stamina")
	EStaminaRecoveryState GetRecoveryState() const { return CurrentRecoveryState; }
	
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void StartRecoveryDelay(float DelaySeconds = -1.f);
	
	UFUNCTION(BlueprintPure, Category = "Stamina")
	bool IsRecoveryDelayed() const { return bIsRecoveryDelayed; }

	// ─────────────────────────────────────────
	// 조회 API
	// ─────────────────────────────────────────
	UFUNCTION(BlueprintPure, Category = "Stamina")
	float GetCurrentStamina() const;
	
	UFUNCTION(BlueprintPure, Category = "Stamina")
	float GetMaxStamina() const;
	
	UFUNCTION(BlueprintPure, Category = "Stamina")
	float GetStaminaPercent() const;

	// ─────────────────────────────────────────
	// 이벤트
	// ─────────────────────────────────────────
	UPROPERTY(BlueprintAssignable, Category = "Stamina")
	FOnStaminaDepletedSignature OnStaminaDepleted;
	
	UPROPERTY(BlueprintAssignable, Category = "Stamina")
	FOnStaminaChangedSignature OnStaminaChanged;

protected:
	virtual void BeginPlay() override;
	
	float GetCurrentRecoveryRate() const;
	void ApplyStaminaChange(float Delta);
	void OnRecoveryDelayEnded();
	void TickRecovery(float DeltaTime);
	void TickContinuousConsumption(float DeltaTime);
	UAbilitySystemComponent* GetASC() const;
	const UKRPlayerAttributeSet* GetPlayerAttributeSet() const;

protected:
	// ─────────────────────────────────────────
	// 회복 설정
	// ─────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, Category = "Stamina|Recovery")
	FKRStaminaRecoveryConfig RecoveryConfig;

private:
	EStaminaRecoveryState CurrentRecoveryState = EStaminaRecoveryState::Normal;

	bool bIsContinuousConsuming = false;
	float ContinuousConsumeRate = 0.0f;

	FTimerHandle RecoveryDelayTimerHandle;
	bool bIsRecoveryDelayed = false;

	float LastStaminaValue = 0.f;

	UPROPERTY()
	mutable TWeakObjectPtr<UAbilitySystemComponent> CachedASC;

	UPROPERTY()
	mutable TWeakObjectPtr<const UKRPlayerAttributeSet> CachedAttributeSet;
};
