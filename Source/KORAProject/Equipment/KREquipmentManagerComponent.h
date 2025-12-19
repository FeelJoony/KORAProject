#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/PawnComponent.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GAS/AbilitySet/KRAbilitySet.h"

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

DECLARE_LOG_CATEGORY_EXTERN(LogEquipmentManagerComponent, Log, All);

USTRUCT(BlueprintType)
struct FKRAppliedEquipmentEntry
{
	GENERATED_BODY()

	FKRAppliedEquipmentEntry()
	{}

	static FKRAppliedEquipmentEntry Invalid_Entry;

	const TArray<TObjectPtr<UAnimMontage>>& GetLightAttackMontages() const { return LightAttackMontages; }
	const TArray<TObjectPtr<UAnimMontage>>& GetChargeAttackMontages() const { return ChargeAttackMontages; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Entry", meta = (AllowPrivateAccess = "true"))
	FGameplayTag TypeTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Entry", meta = (AllowPrivateAccess = "true"))
	TArray<FKRAbilitySet_GameplayAbility> GrantedAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Entry", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UAnimInstance> EquipAnimLayer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Entry", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> EquipIMC;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Entry", meta = (AllowPrivateAccess = "true"))
	int32 InputPriority;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Entry", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<class UAnimMontage>> LightAttackMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Entry", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<class UAnimMontage>> ChargeAttackMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Entry", meta = (AllowPrivateAccess = "true"))
	FName SocketName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Entry", meta = (AllowPrivateAccess = "true"))
	FTransform AttachTransform;

	bool IsValid() const;
	
private:
	friend FKREquipmentList;
	friend UKREquipmentManagerComponent;

	UPROPERTY()
	mutable TObjectPtr<class UKRInventoryItemInstance> ItemInstance;
	
	UPROPERTY()
	mutable FKRAbilitySet_GrantedHandles GrantedHandles;
};

USTRUCT(BlueprintType, Blueprintable)
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


	FKRAppliedEquipmentEntry& FindEntryByItemTag(FGameplayTag InTypeTag);
	const FKRAppliedEquipmentEntry& FindEntryByItemTag(FGameplayTag InTypeTag) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EntryList", meta = (AllowPrivateAccess = "true"))
	TArray<FKRAppliedEquipmentEntry> Entries;
	
private:
	friend UKREquipmentManagerComponent;

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
	void AddEquip(class UKRInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SpawnActorInstances();
	
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void EquipItem(class UKRInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void UnequipItem(class UKRInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void ChangeEquipment(class UKRInventoryItemInstance* OldItem, class UKRInventoryItemInstance* NewItem);
	
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	const FKRAppliedEquipmentEntry& FindEntryByTag(const FGameplayTag& ItemTag) const;

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	UKRInventoryItemInstance* GetEquippedItemInstanceBySlotTag(const FGameplayTag& SlotTag) const;
	
	FORCEINLINE class AKRMeleeWeapon* GetMeleeActorInstance() { return MeleeActorInstance; }
	FORCEINLINE class AKRRangeWeapon* GetRangeActorInstance() { return RangeActorInstance; }
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Equip|Actor", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AKRMeleeWeapon> MeleeActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Equip|Actor", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AKRRangeWeapon> RangeActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equip|Entry", meta = (AllowPrivateAccess = "true"))
	TMap<FGameplayTag, FKREquipmentList> EquipSlotTagToEquipmentListMap;

	void ApplyStatsToASC(const class UInventoryFragment_SetStats* SetStatsFragment, bool bAdd);
	
private:
	UPROPERTY()
	TObjectPtr<class AKRMeleeWeapon> MeleeActorInstance;

	UPROPERTY()
	TObjectPtr<class AKRRangeWeapon> RangeActorInstance;

	UPROPERTY()
	FKRAbilitySet_GrantedHandles GrantedHandles;
	
};