#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "KRCombatComponent.generated.h"

class AKRWeaponBase;

UCLASS()
class KORAPROJECT_API UKRCombatComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UKRCombatComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "KR|Combat")
	void ToggleWeaponCollision(bool bShouldEnable);

	virtual void OnHitTargetActor(AActor* HitActor, const FHitResult& HitResult);

protected:
	AKRWeaponBase* GetCurrentWeapon() const;

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> OverlappedActors;
};
