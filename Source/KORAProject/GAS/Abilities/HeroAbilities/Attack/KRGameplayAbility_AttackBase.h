#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "KRGameplayAbility_AttackBase.generated.h"

UCLASS()
class KORAPROJECT_API UKRGameplayAbility_AttackBase : public UKRGameplayAbility
{
	GENERATED_BODY()

public:
	UKRGameplayAbility_AttackBase(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Combat|Combo")
	int32 CurrentComboIndex;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Combo")
	int32 MaxComboCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Combo")
	float ComboResetTime;

	FTimerHandle ComboResetTimerHandle;

	UFUNCTION(BlueprintCallable, Category = "Combat|Combo")
	void IncrementCombo();

	UFUNCTION(BlueprintCallable, Category = "Combat|Combo")
	void ResetCombo();

	void StartComboResetTimer();
	void OnComboResetTimer();
	bool IsLastCombo() const;
};
