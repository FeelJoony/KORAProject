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
	
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void SetWeaponInstance(UKRWeaponInstance* InInstance);

	UFUNCTION(BlueprintPure, Category="Weapon")
	UKRWeaponInstance* GetWeaponInstance() const { return WeaponInstance; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(BlueprintReadOnly, Category="Weapon")
	TObjectPtr<UKRWeaponInstance> WeaponInstance;

	virtual void OnWeaponInstanceAssigned();
};
