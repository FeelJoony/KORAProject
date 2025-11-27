#pragma once

#include "CoreMinimal.h"
#include "ModularPlayerState.h"
#include "AbilitySystemInterface.h"
#include "KRPlayerState.generated.h"

class UKRCombatCommonSet;
class UKRPlayerAttributeSet;
class UKRAbilitySystemComponent;
class UKRPawnData;

UCLASS()
class KORAPROJECT_API AKRPlayerState : public AModularPlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AKRPlayerState(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintCallable, Category = "KR|PlayerState")
	UKRAbilitySystemComponent* GetKRAbilitySystemComponent() const {return KRASC;}
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	
	// Experience Section
public:
	template<class T>
	const T* GetPawnData() const {return Cast<T>(PawnData); }
	void OnExperienceLoaded(const class UKRExperienceDefinition* CurrentExperience);
	void SetPawnData(const UKRPawnData* InPawnData);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UKRAbilitySystemComponent> KRASC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UKRCombatCommonSet> CombatCommonSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UKRPlayerAttributeSet> PlayerAttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KR|PawnData", ReplicatedUsing = OnRep_PawnData)
	TObjectPtr<const UKRPawnData> PawnData;

	UFUNCTION()
	void OnRep_PawnData();

	bool bStartupGiven = false;

public:
	FORCEINLINE UKRCombatCommonSet* GetCombatCommonSet() const {return CombatCommonSet;}
	FORCEINLINE UKRPlayerAttributeSet* GetPlayerAttributeSet() const {return PlayerAttributeSet;}
	
};
