#pragma once

#include "CoreMinimal.h"
#include "ModularCharacter.h"
#include "AbilitySystemInterface.h"
#include "KRBaseCharacter.generated.h"

class UKRCombatComponent;
class UAbilitySystemComponent;
class UKRPawnExtensionComponent;

UCLASS()
class KORAPROJECT_API AKRBaseCharacter : public AModularCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AKRBaseCharacter(const FObjectInitializer& ObjectInitializer);

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KR|Character")
	TObjectPtr<UKRPawnExtensionComponent> PawnExtensionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "KR|Combat")
	TObjectPtr<UKRCombatComponent> CombatComponent;
};
