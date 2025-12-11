#pragma once

#include "CoreMinimal.h"
#include "Equipment/KREquipmentInstance.h"
#include "GameplayTagContainer.h"
#include "GAS/AbilitySet/KRAbilitySet.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "KRWeaponInstance.generated.h"

class UInputMappingContext;
class UInventoryFragment_SetStats;
class UInventoryFragment_EnhanceableItem;
class UKRInventoryItemInstance;
class AKRBaseCharacter;
class AKRPlayerController;

UCLASS(Blueprintable, BlueprintType)
class KORAPROJECT_API UKRWeaponInstance : public UKREquipmentInstance
{
    GENERATED_BODY()

public:
    UKRWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // Equipment Instance Override
    virtual void OnEquipped(const TArray<FKREquipmentActorToSpawn>& ActorsToSpawn) override;
    virtual void OnUnequipped() override;

    void ActivateWeapon(AKRBaseCharacter* Character, AKRPlayerController* PC, UKRAbilitySystemComponent* ASC, EWeaponMessageAction Action = EWeaponMessageAction::Equipped, bool bBroadcast = true);
    void DeactivateWeapon(AKRBaseCharacter* Character, AKRPlayerController* PC, UKRAbilitySystemComponent* ASC, EWeaponMessageAction Action = EWeaponMessageAction::Unequipped, bool bBroadcast = true);

    void InitializeFromItem(UKRInventoryItemInstance* ItemInstance);
    virtual void InitializeStats(const UInventoryFragment_SetStats* StatsFragment) override;
    
    // 무기 타입
    UPROPERTY(BlueprintReadOnly, Category = "Weapon")
    FGameplayTag WeaponType; // Weapon.Type.Sword, Weapon.Type.Gun

    // 무기 스펙 (캐시)
    UPROPERTY(BlueprintReadOnly, Category = "Weapon|Stats")
    float AttackPower;

    float BaseAttackPower;

    UPROPERTY(BlueprintReadOnly, Category = "Weapon|Stats")
    float CritChance;

    float BaseCritChance;

    UPROPERTY(BlueprintReadOnly, Category = "Weapon|Stats")
    float CritMultiplier;

    UPROPERTY(BlueprintReadOnly, Category = "Weapon|Stats")
    float AttackSpeed;

    // 총 전용 스펙
    UPROPERTY(BlueprintReadOnly, Category = "Weapon|Stats")
    float Range;
    
    UPROPERTY(BlueprintReadOnly, Category = "Weapon|Gun")
    int32 CurrentAmmo;

    UPROPERTY(BlueprintReadOnly, Category = "Weapon|Gun")
    int32 MaxAmmo;

    UPROPERTY(BlueprintReadOnly, Category = "Weapon|Gun")
    float ReloadTime;

    UPROPERTY(BlueprintReadOnly)
    int32 CurrentEnhanceLevel;

    UFUNCTION(BlueprintCallable, Category = "Weapon|Combat")
    float CalculateDamage(bool& bOutIsCritical);

    // 강화 레벨 적용
    virtual void ApplyEnhanceLevel(int32 EnhanceLevel) override;

protected:
    UPROPERTY()
    TSubclassOf<AActor> CachedWeaponActorClass;

    UPROPERTY()
    FName CachedAttachSocket;

    UPROPERTY()
    FTransform CachedAttachTransform;

    UPROPERTY()
    TObjectPtr<AActor> SpawnedWeaponActor;

    UPROPERTY()
    TSubclassOf<UAnimInstance> CachedAnimLayer;
    
    UPROPERTY()
    TObjectPtr<UInputMappingContext> CachedIMC;

    UPROPERTY()
    int32 CachedIMCPriority;

    UPROPERTY()
    TArray<FKRAbilitySet_GameplayAbility> CachedAbilities;
    
    UPROPERTY()
    FKRAbilitySet_GrantedHandles GrantedHandles;

    UPROPERTY(BlueprintReadOnly, Category = "Weapon|Visual")
    TArray<TObjectPtr<UAnimMontage>> LightAttackMontages;

    UPROPERTY(BlueprintReadOnly, Category = "Weapon|Visual")
    TArray<TObjectPtr<UAnimMontage>> ChargeAttackMontages;

    float ChargeTime = 1.f;
public:
    virtual void SpawnEquipmentActors(const TArray<FKREquipmentActorToSpawn>& ActorsToSpawn) override;
    virtual void DestroyEquipmentActors() override;

    UFUNCTION(BlueprintCallable, Category = "Weapon|Visual")
    void ApplyWeaponAnimLayer(AKRBaseCharacter* TargetCharacter);

    UFUNCTION(BlueprintCallable, Category = "Weapon|Visual")
    void RemoveWeaponAnimLayer(AKRBaseCharacter* TargetCharacter);

    UFUNCTION(BlueprintCallable, Category = "Weapon|Visual")
    void AddWeaponInputContext(AKRPlayerController* TargetPC);

    UFUNCTION(BlueprintCallable, Category = "Weapon|Visual")
    void RemoveWeaponInputContext(AKRPlayerController* TargetPC);

    UFUNCTION(BlueprintCallable, Category = "Weapon|Visual")
    UAnimMontage* GetLightAttackMontage(int32 ComboIndex) const;
    
    UFUNCTION(BlueprintCallable, Category = "Weapon|Visual")
    UAnimMontage* GetChargeAttackMontage(int32 ComboIndex) const;

    UFUNCTION(BlueprintCallable, Category = "Weapon|Visual")
    float GetWeaponChargeTime() const;
    
    void GrantWeaponAbilities(UKRAbilitySystemComponent* ASC);
    void RemoveWeaponAbilities(UKRAbilitySystemComponent* ASC);

    void PlayEquipVFX();
    void PlayUnequipVFX();

    void BroadcastWeaponMessage(const EWeaponMessageAction Action);
};