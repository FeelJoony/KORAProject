#pragma once

#include "CoreMinimal.h"
#include "Characters/KRBaseCharacter.h"
#include "GameplayTagContainer.h"
#include "KREnemyCharacter.generated.h"

class UKRCombatCommonSet;
class UKRAbilitySystemComponent;
class UKRCombatComponent;
class UKREnemyAttributeSet;
class UKRPawnData;
class UStateTreeComponent;

UCLASS()
class KORAPROJECT_API AKREnemyCharacter : public AKRBaseCharacter
{
	GENERATED_BODY()

public:
	AKREnemyCharacter(const FObjectInitializer& ObjectInitializer);

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	
protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "KR|GAS")
	TObjectPtr<UKRAbilitySystemComponent> EnemyASC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "KR|GAS")
	TObjectPtr<UKRCombatCommonSet> CombatCommonSet;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "KR|GAS")
	TObjectPtr<UKREnemyAttributeSet> EnemyAttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "KR|Enemy")
	TObjectPtr<const UKRPawnData> PawnData;

public:
	FORCEINLINE UKRAbilitySystemComponent* GetEnemyAbilitySystemCompoent() const { return EnemyASC; }

	UKRCombatComponent* GetEnemyCombatComponent() const { return CombatComponent; }

	UStateTreeComponent* GetStateTreeComponent() const;

	const TArray<FName>& GetLockOnSockets() const { return LockOnSockets; }

	UPROPERTY(EditDefaultsOnly, Category = KRMontage)
	TObjectPtr<UAnimMontage> StunMontage;

	UPROPERTY(EditDefaultsOnly, Category = KRMontage)
	TObjectPtr<UAnimMontage> HitMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "KR|Combat")
	TArray<FName> LockOnSockets;

private:
	UFUNCTION()
	void RegisterTagEvent();

	UFUNCTION()
	void HandleTagEvent(FGameplayTag Tag, int32 Count);

	UFUNCTION()
	void SetEnemyState(FGameplayTag StateTag);

	UFUNCTION()
	void ExternalGAEnded(FGameplayTag Tag);

	UFUNCTION()
	void OnGEAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle);


	TArray<FGameplayTag> StateTags;
};
