// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "ModularCharacter.h"
#include "KREnemyPawn.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSelectAttack, TSubclassOf<class UGameplayAbility>, float);
DECLARE_MULTICAST_DELEGATE(FOnUnselectAttack);

UCLASS()
class KORAPROJECT_API AKREnemyPawn : public AModularCharacter, public IAbilitySystemInterface 
{
	GENERATED_BODY()

public:
	AKREnemyPawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override; 

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void InitializeComponents();

	UFUNCTION(BlueprintCallable)
	bool TryActivateAbility(TSubclassOf<class UGameplayAbility> AbilityClass);

	FORCEINLINE FGameplayTag GetEnemyTag() const { return EnemyTag; }

	FOnSelectAttack OnSelectAttack;
	FOnUnselectAttack OnUnselectAttack;

	UPROPERTY()
	TObjectPtr<AActor> TargetActor = nullptr;

protected:
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	// TObjectPtr<class UCapsuleComponent> CapsuleComponent;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	// TObjectPtr<class USkeletalMeshComponent> Mesh;
	//    	
	// UPROPERTY()
	// TObjectPtr<class UArrowComponent> ArrowComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tag, meta = (categories = "Enemy.Type"))
	FGameplayTag EnemyTag; 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GAS)
	TObjectPtr<class UKRAbilitySystemComponent> EnemyASC;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	TObjectPtr<class UKRCombatComponent> CombatComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GAS)
	TObjectPtr<class UKRCombatCommonSet> CombatCommonSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GAS)
	TObjectPtr<class UKREnemyAttributeSet> EnemyAttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GAS)
	TArray<TSubclassOf<class UGameplayAbility>> ApplyAbilityClasses;

protected:
	// UPROPERTY(EditDefaultsOnly, Category = Animation)
	// TObjectPtr<UAnimMontage> StunMontage;
	//
	// UPROPERTY(EditDefaultsOnly, Category = Animation)
	// TObjectPtr<UAnimMontage> HitMontage;
	//
	// UPROPERTY(EditDefaultsOnly, Category = Animation)
	// TObjectPtr<UAnimMontage> DieMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WBP)
	TObjectPtr<class UWidgetComponent> HPWidgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = LockOn)
	TArray<FName> LockOnSockets;

private:
	template<typename TDataType>
	const TDataType* FindDataByLoad(const FString& Path)
	{
		UDataTable* DataTable = Cast<class UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *Path));

		TArray<TDataType*> DataArray;
		DataTable->GetAllRows(TEXT(""), DataArray);

		for (const TDataType* Data : DataArray)
		{
			if (Data->EnemyTag == EnemyTag)
			{
				return Data;
			}
		}

		return nullptr;
	}
};
