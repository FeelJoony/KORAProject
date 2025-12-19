#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "KRWeaponBase.generated.h"

class UStaticMeshComponent;
class UKRInventoryItemInstance;
class UNiagaraSystem;

UCLASS()
class KORAPROJECT_API AKRWeaponBase : public AActor
{
	GENERATED_BODY()

public:	
	AKRWeaponBase();

	void SetWeaponVisibility(bool bVisible);

	virtual void ConfigureWeapon(UKRInventoryItemInstance* InInstance);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayEquipEffect();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayUnequipEffect();
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	FGameplayTag GetWeaponItemTag() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TObjectPtr<UKRInventoryItemInstance> ItemInstance;
	
};
