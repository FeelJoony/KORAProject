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

	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;

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

	// 무기 액터를 캐릭터 소켓에 Attach (숨김 상태로)
	UFUNCTION(BlueprintCallable, Category = "Equipment|Weapon")
	void AttachWeaponsToCharacter();

	// 전투 모드 활성화 - GA 부여, IMC 추가, AnimLayer 변경, 무기 가시성 활성화
	UFUNCTION(BlueprintCallable, Category = "Equipment|Combat")
	bool ActivateCombatMode(const FGameplayTag& WeaponTypeTag);

	// 전투 모드 비활성화 - GA 제거, IMC 제거, AnimLayer 복원, 무기 가시성 비활성화
	UFUNCTION(BlueprintCallable, Category = "Equipment|Combat")
	void DeactivateCombatMode();

	// 현재 전투 모드가 활성화되어 있는지
	UFUNCTION(BlueprintCallable, Category = "Equipment|Combat")
	bool IsCombatModeActive() const { return bCombatModeActive; }

	// 현재 활성화된 무기 타입 태그
	UFUNCTION(BlueprintCallable, Category = "Equipment|Combat")
	FGameplayTag GetActiveWeaponTypeTag() const { return ActiveWeaponTypeTag; }

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
	void OnAbilitySystemInitialized();

	// EquipItem 호출 시 EquipInstance가 없으면 생성
	// Fragment가 Manager를 호출하는 역방향 의존성 제거를 위해 Manager가 주도권을 가짐
	void EnsureEquipInstanceCreated(class UKRInventoryItemInstance* ItemInstance);

	// ASC 초기화 콜백 중복 호출 방지 플래그
	bool bASCInitCallbackProcessed = false;

	UPROPERTY()
	TObjectPtr<class AKRMeleeWeapon> MeleeActorInstance;

	UPROPERTY()
	TObjectPtr<class AKRRangeWeapon> RangeActorInstance;

	UPROPERTY()
	FKRAbilitySet_GrantedHandles GrantedHandles;

	// 전투 모드 상태 추적
	bool bCombatModeActive = false;
	FGameplayTag ActiveWeaponTypeTag;
	FKRAbilitySet_GrantedHandles CombatModeGrantedHandles;
	TSubclassOf<class UAnimInstance> ActiveAnimLayer;
	TObjectPtr<class UInputMappingContext> ActiveIMC;
};