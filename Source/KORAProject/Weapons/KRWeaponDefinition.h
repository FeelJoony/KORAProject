#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "KRWeaponDefinition.generated.h"

class UGameplayAbility;
class UKRInventoryItemDefinition;
class UKREquipmentDefinition;
class UKRWeaponInstance;
struct FWeaponDataStruct;
struct FWeaponEnhanceDataStruct;

/**
 * 무기 Definition DataAsset
 * DataTable의 무기 데이터를 기반으로 ItemDefinition과 EquipmentDefinition을 생성
 */
UCLASS(BlueprintType)
class KORAPROJECT_API UKRWeaponDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UKRWeaponDefinition();
	
	// DataTable Row Name (무기 식별자)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName WeaponID;

	// 무기 타입 (검, 총 등)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FGameplayTag WeaponTypeTag; // ex) Weapon.Type.Sword, Weapon.Type.Gun

	// Equipment Definition (실제 장착 시스템)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TSubclassOf<UKREquipmentDefinition> EquipmentDefinition;

	// Weapon Instance 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TSubclassOf<UKRWeaponInstance> WeaponInstanceClass;

	// 부여할 어빌리티 (공격, 특수 기술 등)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;

	// 스폰할 액터 (무기 메시 등)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	TSubclassOf<AActor> WeaponActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	FName AttachSocketName = "WeaponSocket";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	FTransform AttachTransform;

	// 생성 함수
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	UKRInventoryItemDefinition* CreateInventoryItemDefinition(const FWeaponDataStruct& WeaponData, const FWeaponEnhanceDataStruct& WeaponEnhanceData);
};