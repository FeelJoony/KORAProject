#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KRWeaponBase.generated.h"

class USkeletalMeshComponent;
class UKRWeaponInstance;

UCLASS()
class KORAPROJECT_API AKRWeaponBase : public AActor
{
	GENERATED_BODY()

public:	
	AKRWeaponBase();

	UFUNCTION(BlueprintPure, Category="Weapon")
	virtual FTransform GetMuzzleTransform() const;

	UFUNCTION(BlueprintCallable, Category="Weapon")
	void SetWeaponInstance(UKRWeaponInstance* InInstance);

	UFUNCTION(BlueprintPure, Category="Weapon")
	UKRWeaponInstance* GetWeaponInstance() const { return WeaponInstance; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(BlueprintReadOnly, Category="Weapon")
	TObjectPtr<UKRWeaponInstance> WeaponInstance;

	virtual void OnWeaponInstanceAssigned();
};
