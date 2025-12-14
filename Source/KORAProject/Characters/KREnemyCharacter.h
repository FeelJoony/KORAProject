#pragma once

#include "CoreMinimal.h"
#include "Characters/KRBaseCharacter.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "KREnemyCharacter.generated.h"

class UKRCombatCommonSet;
class UKRAbilitySystemComponent;
class UKRCombatComponent;
class UKREnemyAttributeSet;
class UKRPawnData;
class UStateTreeComponent;
class UWidgetComponent;

UCLASS()
class KORAPROJECT_API AKREnemyCharacter : public AKRBaseCharacter
{
	GENERATED_BODY()

public:
	AKREnemyCharacter(const FObjectInitializer& ObjectInitializer);

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "AI|Speed")
	void SetPatrolSpeed();

	UFUNCTION(BlueprintCallable, Category = "AI|Speed")
	void SetAlertSpeed();

	UFUNCTION(BlueprintCallable, Category = "AI|Speed")
	void SetChaseSpeed();
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "KR|GAS")
	TObjectPtr<UKRAbilitySystemComponent> EnemyASC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "KR|GAS")
	TObjectPtr<UKRCombatCommonSet> CombatCommonSet;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "KR|GAS")
	TObjectPtr<UKREnemyAttributeSet> EnemyAttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "KR|Enemy")
	TObjectPtr<const UKRPawnData> PawnData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Speed")
	float PatrolSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Speed")
	float AlertSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Speed")
	float ChaseSpeed;

public:
	FORCEINLINE UKRAbilitySystemComponent* GetEnemyAbilitySystemCompoent() const { return EnemyASC; }

	UKRCombatComponent* GetEnemyCombatComponent() const { return CombatComponent; }

	UStateTreeComponent* GetStateTreeComponent() const;

	const TArray<FName>& GetLockOnSockets() const { return LockOnSockets; }

	UPROPERTY(EditDefaultsOnly, Category = KRMontage)
	TObjectPtr<UAnimMontage> StunMontage;

	UPROPERTY(EditDefaultsOnly, Category = KRMontage)
	TObjectPtr<UAnimMontage> HitMontage;

	UPROPERTY(EditDefaultsOnly, Category = KRMontage)
	TObjectPtr<UAnimMontage> DieMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "KR|Combat")
	TArray<FName> LockOnSockets;

	UWidgetComponent* HPWidgetComp;

private:
	UFUNCTION()
	void RegisterTagEvent();

	UFUNCTION()
	void HandleTagEvent(FGameplayTag Tag, int32 Count);

	UFUNCTION()
	void SetEnemyState(FGameplayTag StateTag);

	UFUNCTION()
	void ExternalGAEnded(FGameplayTag Tag);

	void OnGEAdded(const FOnAttributeChangeData& Data);

	TArray<FGameplayTag> StateTags;
};
