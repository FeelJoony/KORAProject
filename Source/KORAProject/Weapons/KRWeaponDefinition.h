#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GAS/AbilitySet/KRAbilitySet.h"
#include "KRWeaponDefinition.generated.h"

class UInputMappingContext;
class UKRInventoryItemDefinition;
class UKREquipmentDefinition;
class UKRWeaponInstance;
struct FWeaponDataStruct;
struct FWeaponEnhanceDataStruct;

/**
 * 무기 Definition (Melee / Range 공통)
 */
UCLASS(BlueprintType)
class KORAPROJECT_API UKRWeaponDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UKRWeaponDefinition();

    /** 무기 고유 ID (DataTable row) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FName WeaponID;

    /** 무기 타입 (ex. Weapon.Type.Melee, Weapon.Type.Range) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FGameplayTag WeaponTypeTag;

    /** 인스턴스 클래스 (ex. UKRMeleeWeaponInstance, UKRRangeWeaponInstance) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    TSubclassOf<UKRWeaponInstance> WeaponInstanceClass;

    /** 무기 Actor 클래스 (ex. AKRMeleeWeapon, AKRRangeWeapon) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    TSubclassOf<AActor> WeaponActorClass;

    /** 장착 시스템 정보 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    TSubclassOf<UKREquipmentDefinition> EquipmentDefinition;

    /** 장착 소켓 이름 (예: WeaponSocket) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
    FName AttachSocketName = "WeaponSocket";

    /** 액터 스폰 위치/회전 보정값 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
    FTransform AttachTransform;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
    TSubclassOf<UAnimInstance> EquippableAnimLayer;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> WeaponIMC;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    int32 InputPriority = 1;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
    TArray<FKRAbilitySet_GameplayAbility> GrantedAbilities;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
    TArray<TObjectPtr<UAnimMontage>> LightAttackMontages;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
    TArray<TObjectPtr<UAnimMontage>> ChargeAttackMontages;
    
    /** 인벤토리 아이템 정의 생성 */
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    UKRInventoryItemDefinition* CreateInventoryItemDefinition(const FWeaponDataStruct& WeaponData, const FWeaponEnhanceDataStruct& WeaponEnhanceData);
    
    void AutoAssignAllClasses();
};
