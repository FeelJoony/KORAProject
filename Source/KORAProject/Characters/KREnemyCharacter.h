#pragma once

#include "CoreMinimal.h"
#include "Characters/KRBaseCharacter.h"
#include "KREnemyCharacter.generated.h"

class UKRCombatCommonSet;
class UKRAbilitySystemComponent;
class UEnemyCombatComponent;
class UKREnemyAttributeSet;
class UAbilitySystemComponent;

UCLASS()
class KORAPROJECT_API AKREnemyCharacter : public AKRBaseCharacter
{
	GENERATED_BODY()

public:
	AKREnemyCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	
protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Controller() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UEnemyCombatComponent> EnemyCombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "GAS")
	TObjectPtr<UKRAbilitySystemComponent> EnemyASC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UKRCombatCommonSet> CombatCommonSet;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UKREnemyAttributeSet> EnemyAttributeSet;


private:
	void InitEnemyASC();
	void InitEnemyStartUpData();
	
public:
	FORCEINLINE UKRAbilitySystemComponent* GetEnemyAbilitySystemCompoent() const { return EnemyASC; }
	FORCEINLINE UEnemyCombatComponent* GetEnemyCombatComponent() const { return EnemyCombatComponent; }
	FORCEINLINE UKRCombatCommonSet* GetCombatCommonSet() const { return CombatCommonSet; }
	FORCEINLINE UKREnemyAttributeSet* GetEnemyAttributeSet() const { return EnemyAttributeSet; }
};
