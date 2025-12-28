#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/KRGuardTypes.h"
#include "KRGuardRegainComponent.generated.h"

class UAbilitySystemComponent;
class UKRPlayerAttributeSet;
class UKRCombatCommonSet;
struct FGameplayEventData;

// 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGreyHPChangedSignature, float, CurrentGreyHP, float, MaxGreyHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGreyHPRecoveredSignature, float, RecoveredAmount);

/**
 * 가드 리게인 관리 컴포넌트
 * - 일반 가드 시 GreyHP 생성 (GEExecCalc에서 처리)
 * - 공격 성공 시 GreyHP → HP 전환
 * - 시간 경과 시 GreyHP 자연 감소
 * - 피격 시 GreyHP 즉시 소멸
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class KORAPROJECT_API UKRGuardRegainComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UKRGuardRegainComponent();

	// ─────────────────────────────────────────
	// 조회 API
	// ─────────────────────────────────────────
	UFUNCTION(BlueprintPure, Category = "GuardRegain")
	float GetCurrentGreyHP() const;
	
	UFUNCTION(BlueprintPure, Category = "GuardRegain")
	float GetMaxGreyHP() const;
	
	UFUNCTION(BlueprintPure, Category = "GuardRegain")
	bool HasGreyHP() const;

	// ─────────────────────────────────────────
	// 회복 API (외부 호출용)
	// ─────────────────────────────────────────
	UFUNCTION(BlueprintCallable, Category = "GuardRegain")
	void RecoverGreyHPOnAttack(float DamageDealt);
	
	UFUNCTION(BlueprintCallable, Category = "GuardRegain")
	void ClearAllGreyHP();

	// ─────────────────────────────────────────
	// 이벤트
	// ─────────────────────────────────────────
	UPROPERTY(BlueprintAssignable, Category = "GuardRegain")
	FOnGreyHPChangedSignature OnGreyHPChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "GuardRegain")
	FOnGreyHPRecoveredSignature OnGreyHPRecovered;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void SetupEventListeners();
	void ApplyGreyHPDecay(float DeltaTime);
	void SetGreyHP(float NewValue);
	void RecoverHealth(float Amount);

	UAbilitySystemComponent* GetASC() const;
	const UKRPlayerAttributeSet* GetPlayerAttributeSet() const;
	const UKRCombatCommonSet* GetCombatCommonSet() const;

	// ─────────────────────────────────────────
	// 이벤트 핸들러
	// ─────────────────────────────────────────
	void OnDamageDealtEvent(const FGameplayEventData* EventData);
	void OnDamageTakenEvent(const FGameplayEventData* EventData);

protected:
	// ─────────────────────────────────────────
	// 설정 (KRGuardTypes.h의 FKRGuardConfig 참조)
	// ─────────────────────────────────────────

	/** GreyHP 자연 감소 딜레이 (초) */
	UPROPERTY(EditDefaultsOnly, Category = "GuardRegain|Decay")
	float GreyHPDecayDelay = 3.0f;

	/** GreyHP 자연 감소 속도 (초당) */
	UPROPERTY(EditDefaultsOnly, Category = "GuardRegain|Decay")
	float GreyHPDecayRate = 10.0f;

	/** 공격 시 GreyHP 회복률 (데미지 대비) */
	UPROPERTY(EditDefaultsOnly, Category = "GuardRegain|Recovery")
	float GreyHPRecoveryRate = 0.5f;

private:
	/** 마지막 GreyHP 값 (변경 감지용) */
	float LastGreyHPValue = 0.f;

	/** Decay 딜레이 카운터 */
	float DecayDelayRemaining = 0.f;

	/** Decay 활성화 여부 */
	bool bDecayActive = false;

	UPROPERTY()
	mutable TWeakObjectPtr<UAbilitySystemComponent> CachedASC;

	UPROPERTY()
	mutable TWeakObjectPtr<const UKRPlayerAttributeSet> CachedPlayerAttributeSet;

	UPROPERTY()
	mutable TWeakObjectPtr<const UKRCombatCommonSet> CachedCombatCommonSet;

	FDelegateHandle OnHitDelegateHandle;
	FDelegateHandle OnDamageTakenDelegateHandle;
};
