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
	
	void SetOwningInstance(UKRWeaponInstance* InInstance) { OwningInstance = InInstance; }

	void SetWeaponVisibility(bool bVisible);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY()
	TObjectPtr<UKRWeaponInstance> OwningInstance;
	
};
