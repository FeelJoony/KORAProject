#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/PawnComponent.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbilityTypes.h"

#include "KREquipmentManagerComponent.generated.h"

class UActorComponent;
class UKREquipmentDefinition;
class UKREquipmentInstance;
class UKREquipmentManagerComponent;
class UObject;
class UAbilitySystemComponent;
class UKRInventoryItemInstance; // Debug로 잠시 전방 선언
struct FGameplayAbilitySpecHandle;
struct FFrame;
struct FKREquipmentList;

USTRUCT(BlueprintType)
struct FKRAppliedEquipmentEntry
{
	GENERATED_BODY()

	FKRAppliedEquipmentEntry()
	{}

private:
	friend FKREquipmentList;
	friend UKREquipmentManagerComponent;
	
	UPROPERTY()
	TSubclassOf<UKREquipmentDefinition> EquipmentDefinition;
	
	UPROPERTY()
	TObjectPtr<UKREquipmentInstance> Instance = nullptr;
	
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> GrantedAbilityHandles;
};

USTRUCT(BlueprintType)
struct FKREquipmentList
{
	GENERATED_BODY()

	FKREquipmentList()
		: OwnerComponent(nullptr)
	{
	}

	FKREquipmentList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

public:
	FKRAppliedEquipmentEntry* AddEntry(TSubclassOf<UKREquipmentDefinition> InEquipmentDefinition);

	void RemoveEntry(UKREquipmentInstance* InInstance);

	FKRAppliedEquipmentEntry* FindEntryByInstance(UKREquipmentInstance* InInstance);

private:
	friend UKREquipmentManagerComponent;

	UPROPERTY()
	TArray<FKRAppliedEquipmentEntry> Entries;

	UPROPERTY()
	TObjectPtr<UActorComponent> OwnerComponent;
};

UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class KORAPROJECT_API UKREquipmentManagerComponent : public UPawnComponent, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	UKREquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	UKREquipmentInstance* EquipItem(TSubclassOf<UKREquipmentDefinition> InEquipmentDefinition);

	UFUNCTION(BlueprintCallable, Category="Equipment")
	UKREquipmentInstance* EquipFromInventory(class UKRInventoryItemInstance* ItemInstance);
	
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void UnequipItem(UKREquipmentInstance* InItemInstance);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment")
	UKREquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UKREquipmentInstance> InstanceType) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment")
	TArray<UKREquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<UKREquipmentInstance> InstanceType) const;

	template <typename T>
	T* GetFirstInstanceOfType() const
	{
		return (T*)GetFirstInstanceOfType(T::StaticClass());
	}

	// --------Debug--------
	UFUNCTION(BlueprintCallable, Category = "Debug") 
	void Debug_TestEquip(FGameplayTag InItemTag);

private:
	UPROPERTY()
	FKREquipmentList EquipmentList;

	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UKREquipmentInstance>> EquippedSlotsMap;
};