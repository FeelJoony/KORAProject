#pragma once

#include "CoreMinimal.h"
#include "Characters/KRBaseCharacter.h"
#include "KREnemyCharacter.generated.h"

class UKRCombatCommonSet;
class UKRAbilitySystemComponent;
class UKRCombatComponent;
class UKREnemyAttributeSet;
class UKRPawnData;

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
};
