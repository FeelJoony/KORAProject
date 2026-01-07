#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/KRCoreDriveTypes.h"
#include "GameplayTagContainer.h"
#include "KRCoreDriveComponent.generated.h"

class UAbilitySystemComponent;
class UKRPlayerAttributeSet;
struct FGameplayEventData;

/**
 * 코어드라이브 관리 컴포넌트
 * - 공격 히트, 퍼펙트 가드, 적 처치 시 충전
 * - 이벤트 기반 충전 시스템
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class KORAPROJECT_API UKRCoreDriveComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UKRCoreDriveComponent();

	// ─────────────────────────────────────────
	// 충전 API
	// ─────────────────────────────────────────
	UFUNCTION(BlueprintCallable, Category = "CoreDrive")
	void ChargeCoreDrive(float Amount);
	
	UFUNCTION(BlueprintCallable, Category = "CoreDrive")
	void ChargeOnHit(float DamageDealt);
	
	UFUNCTION(BlueprintCallable, Category = "CoreDrive")
	void ChargeOnPerfectGuard();
	
	UFUNCTION(BlueprintCallable, Category = "CoreDrive")
	void ChargeOnKill();

	// ─────────────────────────────────────────
	// 소모 API
	// ─────────────────────────────────────────
	UFUNCTION(BlueprintCallable, Category = "CoreDrive")
	bool ConsumeCoreDrive(float Amount);
	
	UFUNCTION(BlueprintCallable, Category = "CoreDrive")
	bool ConsumeAllCoreDrive();
	
	UFUNCTION(BlueprintPure, Category = "CoreDrive")
	bool HasEnoughCoreDrive(float Amount) const;

	// ─────────────────────────────────────────
	// 조회 API
	// ─────────────────────────────────────────
	UFUNCTION(BlueprintPure, Category = "CoreDrive")
	float GetCurrentCoreDrive() const;
	
	UFUNCTION(BlueprintPure, Category = "CoreDrive")
	float GetMaxCoreDrive() const;
	
	UFUNCTION(BlueprintPure, Category = "CoreDrive")
	float GetCoreDrivePercent() const;
	
	UFUNCTION(BlueprintPure, Category = "CoreDrive")
	bool CanUseCoreDriveBurst() const;

	// ─────────────────────────────────────────
	// 이벤트
	// ─────────────────────────────────────────
	UPROPERTY(BlueprintAssignable, Category = "CoreDrive")
	FOnCoreDriveFullSignature OnCoreDriveFull;
	
	UPROPERTY(BlueprintAssignable, Category = "CoreDrive")
	FOnCoreDriveChangedSignature OnCoreDriveChanged;

protected:
	virtual void BeginPlay() override;
	
	void SetupEventListeners();
	void ApplyCoreDriveChange(float Delta);

	UAbilitySystemComponent* GetASC() const;
	const UKRPlayerAttributeSet* GetPlayerAttributeSet() const;

	// ─────────────────────────────────────────
	// 이벤트 핸들러
	// ─────────────────────────────────────────
	void OnDamageDealtEvent(const FGameplayEventData* EventData);
	void OnPerfectGuardEvent(const FGameplayEventData* EventData);
	void OnEnemyKilledEvent(const FGameplayEventData* EventData);

protected:
	// ─────────────────────────────────────────
	// 충전 설정
	// ─────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Charge")
	FKRCoreDriveChargeConfig ChargeConfig;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Burst")
	FKRCoreDriveBurstConfig BurstConfig;

private:
	float LastCoreDriveValue = 0.f;
	bool bWasFullLastFrame = false;

	UPROPERTY()
	mutable TWeakObjectPtr<UAbilitySystemComponent> CachedASC;

	UPROPERTY()
	mutable TWeakObjectPtr<const UKRPlayerAttributeSet> CachedAttributeSet;

	FDelegateHandle OnHitDelegateHandle;
	FDelegateHandle OnParryDelegateHandle;
	FDelegateHandle OnKillDelegateHandle;
};
