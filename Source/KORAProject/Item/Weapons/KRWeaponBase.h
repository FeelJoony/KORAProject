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

	void SetOwningWeaponInstance(UKRWeaponInstance* InInstance) { OwningWeaponInstance = InInstance; }

	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	USkeletalMeshComponent* WeaponMesh;

	/** 이 무기를 소유한 WeaponInstance Melee Instance ? Ranged Instance?  */
	UPROPERTY()
	TObjectPtr<UKRWeaponInstance> OwningWeaponInstance;
};
