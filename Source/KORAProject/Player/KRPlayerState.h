#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "KRPlayerState.generated.h"

class APawn;
class UKRPlayerAbilitySystemComponent;

UCLASS()
class KORAPROJECT_API AKRPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AKRPlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void InitializeAbilitySystemForPawn(APawn* NewPawn);

	UFUNCTION(BlueprintPure, Category = "GAS")
	UKRPlayerAbilitySystemComponent* GetKRPlayerASC() const {return KRPlayerASC;}

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UKRPlayerAbilitySystemComponent> KRPlayerASC;

	bool bStartupGiven = false;
};
