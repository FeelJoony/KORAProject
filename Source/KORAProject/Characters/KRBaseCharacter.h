#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "KRBaseCharacter.generated.h"

class UAbilitySystemComponent;

UCLASS()
class KORAPROJECT_API AKRBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AKRBaseCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> CachedASC = nullptr;

	FORCEINLINE void SetCachedASC(UAbilitySystemComponent* InASC) { CachedASC = InASC; }
};
