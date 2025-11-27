#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "KRPlayerState.generated.h"

class UKRCombatCommonSet;
class UKRPlayerAttributeSet;
class APawn;
class UKRPlayerAbilitySystemComponent;

UCLASS()
class KORAPROJECT_API AKRPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AKRPlayerState();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void InitASCForAvatar(AActor* NewAvatar);

	UFUNCTION(BlueprintPure, Category = "GAS")
	UKRPlayerAbilitySystemComponent* GetKRPlayerASC() const {return PlayerASC;}


// Experience Section
public:
	template<typename T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }
	void OnExperienceLoaded(const class UKRExperienceDefinition* CurrentExperience);
	void SetPawnData(const class UKRPawnData* NewPawnData);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UKRPlayerAbilitySystemComponent> PlayerASC;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UKRCombatCommonSet> CombatCommonSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UKRPlayerAttributeSet> PlayerAttributeSet;

	UPROPERTY()
	TObjectPtr<const class UKRPawnData> PawnData;
	
	bool bStartupGiven = false;

public:
	FORCEINLINE UKRPlayerAbilitySystemComponent* GetPlayerAbilitySystemComponent() const {return PlayerASC;}
	FORCEINLINE UKRCombatCommonSet* GetCombatCommonSet() const {return CombatCommonSet;}
	FORCEINLINE UKRPlayerAttributeSet* GetPlayerAttributeSet() const {return PlayerAttributeSet;}
	
};
