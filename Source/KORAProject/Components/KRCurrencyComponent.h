#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "KRCurrencyComponent.generated.h"

class UKRAbilitySystemComponent;
class UKRDataTablesSubsystem;
struct FCurrencyDataStruct;

DECLARE_LOG_CATEGORY_EXTERN(LogKRCurrency, Log, All);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
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
	
	UFUNCTION(BlueprintCallable, Category="Currency|UI")
	void ForceBroadcastCurrencyUI();

public:
	UPROPERTY(BlueprintReadOnly, Category="Currency|Insurance")
	float InsuranceKeepRate = 0.0f;
	
protected:
	virtual void BeginPlay() override;
	
	void InitializeAbilityReferences();
	void InitializeDataTables();
	void InitializeCurrencyLossRules();
	
	bool IsLostOnDeath(const FGameplayTag& CurrencyTag) const;
	float GetInsuranceKeepRate() const;
	void ConsumeInsuranceEffects();
	void ApplyCurrencyChange_Internal(const FGameplayTag& CurrencyTag, int32 NewValue);
	void BroadcastCurrencyUI() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Currency|Internal")
	int32 GearingCurrent = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Currency|Internal")
	int32 CorbyteCurrent = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Currency|Internal")
	int32 GearingLost = 0;
	
	UPROPERTY()
	TObjectPtr<UKRAbilitySystemComponent> ASC = nullptr;
	
	UPROPERTY()
	TObjectPtr<UKRDataTablesSubsystem> DataTables = nullptr;
	
	UPROPERTY()
	TMap<FGameplayTag, bool> CurrencyLossRuleMap;

	bool bCurrencyTableInitialized = false;
};
