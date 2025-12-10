#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Data/CurrencyDataStruct.h"
#include "KRCurrencyComponent.generated.h"

class UKRAbilitySystemComponent;
class UKRDataTablesSubsystem;
class UKRPlayerAttributeSet;
class AKRPlayerState;

DECLARE_LOG_CATEGORY_EXTERN(LogKRCurrency, Log, All);

UCLASS()
class KORAPROJECT_API UKRCurrencyComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UKRCurrencyComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="Currency")
	int32 GetCurrency(const FGameplayTag& CurrencyTag) const;

	UFUNCTION(BlueprintPure, Category="Currency")
	int32 GetLostCurrency(const FGameplayTag& CurrencyTag) const;
	
	UFUNCTION(BlueprintCallable, Category="Currency")
	bool CanAfford(const FGameplayTag& CurrencyTag, int32 Amount) const;
	
	UFUNCTION(BlueprintCallable, Category="Currency")
	void AddCurrency(const FGameplayTag& CurrencyTag, int32 Delta);
	
	UFUNCTION(BlueprintCallable, Category="Currency")
	bool SpendCurrency(const FGameplayTag& CurrencyTag, int32 Cost);

	UFUNCTION(BlueprintCallable, Category="Currency|Insurance")
	void SetInsuranceKeepRate(float NewRate);

	UFUNCTION(BlueprintCallable, Category="Currency|Insurance")
	void ClearInsurance();
	
	UFUNCTION(BlueprintCallable, Category="Currency")
	void HandleDeath();

public:
	UPROPERTY(BlueprintReadOnly, Category="Currency|Insurance")
	float InsuranceKeepRate = 0.0f;
	
protected:
	virtual void BeginPlay() override;
	
	void InitializeAbilityReferences();
	void InitializeDataTables();
	
	bool GetAttributeForCurrencyTag(const FGameplayTag& CurrencyTag, struct FGameplayAttribute& OutAttribute) const;
	bool GetLostAttributeForCurrencyTag(const FGameplayTag& CurrencyTag, struct FGameplayAttribute& OutAttribute) const;
	bool IsLostOnDeath(const FGameplayTag& CurrencyTag) const;
	void ApplyCurrencyChange(const FGameplayTag& CurrencyTag, int32 NewValue);

	float GetInsuranceKeepRate() const;
	void ConsumeInsuranceEffects();
	void InitializeCurrencyLossRules();
	
	UPROPERTY()
	TObjectPtr<UKRAbilitySystemComponent> ASC = nullptr;

	UPROPERTY()
	UKRDataTablesSubsystem* DataTables = nullptr;
	
	UPROPERTY()
	TMap<FGameplayTag, bool> CurrencyLossRuleMap;

	bool bCurrencyTableInitialized = false;
	
	const UKRPlayerAttributeSet* AttributeSet = nullptr;
};
