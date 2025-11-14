#pragma once

#include "CoreMinimal.h"
#include "Characters/KRBaseCharacter.h"
#include "KREnemyCharacter.generated.h"

class UKRAbilitySystemComponent;
class UEnemyCombatComponent;

UCLASS()
class KORAPROJECT_API AKREnemyCharacter : public AKRBaseCharacter
{
	GENERATED_BODY()

public:
	AKREnemyCharacter();

protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Controller() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UEnemyCombatComponent> EnemyCombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "GAS")
	TObjectPtr<UKRAbilitySystemComponent> EnemyASC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UKRAttributeSet> EnemyAttributeSet;


private:
	void InitEnemyASC();
	void InitEnemyStartUpData();
	
public:
	FORCEINLINE UEnemyCombatComponent* GetEnemyCombatComponent() const { return EnemyCombatComponent; }
};
