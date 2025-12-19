#pragma once

#include "CoreMinimal.h"
#include "Item/Weapons/KRWeaponBase.h"
#include "KRMeleeWeapon.generated.h"

class UBoxComponent;

UCLASS()
class KORAPROJECT_API AKRMeleeWeapon : public AKRWeaponBase
{
	GENERATED_BODY()

public:
	AKRMeleeWeapon();

	virtual void BeginPlay() override; 

	virtual void ConfigureWeapon(UKRInventoryItemInstance* InInstance) override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons")
	TObjectPtr<UBoxComponent> WeaponCollisionBox;

	UFUNCTION()
	virtual void OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	
public:
	FORCEINLINE UBoxComponent* GetWeaponCollisionBox() const {return WeaponCollisionBox;}
	void SetHitCollisionEnabled(bool bEnabled);
};
