#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "KRPlayerState.generated.h"

class UKRAttributeSet;
class APawn;
class UKRPlayerAbilitySystemComponent;
class UKRAbilitySystemComponent;

UCLASS()
class KORAPROJECT_API AKRPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AKRPlayerState();
	virtual void BeginPlay() override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void InitASCForAvatar(AActor* NewAvatar);

	UFUNCTION(BlueprintPure, Category = "GAS")
	UKRAbilitySystemComponent* GetKRPlayerASC() const {return KRPlayerASC;}

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UKRAbilitySystemComponent> KRPlayerASC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UKRAttributeSet> KRAttributeSet;
	
	bool bStartupGiven = false;

public:
	FORCEINLINE UKRAbilitySystemComponent* GetKRPlayerAbilitySystemComponent() const {return KRPlayerASC;}
	FORCEINLINE UKRAttributeSet* GetKRAttributeSet() const {return KRAttributeSet;}
	
};
