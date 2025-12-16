#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KRWeaponBase.generated.h"

class UStaticMeshComponent;
class UKRWeaponInstance;
class UNiagaraSystem;

UCLASS()
class KORAPROJECT_API AKRWeaponBase : public AActor
{
	GENERATED_BODY()

public:	
	AKRWeaponBase();

	void SetWeaponVisibility(bool bVisible);

	virtual void ConfigureWeapon(const class UInventoryFragment_EquippableItem* Equippable, const class UInventoryFragment_SetStats* SetStat) {}
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void PlayEquipEffect();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable) void PlayUnequipEffect();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TObjectPtr<class UInventoryFragment_EquippableItem> EquippableFragment;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TObjectPtr<class UInventoryFragment_SetStats> SetStatFragment;
};
