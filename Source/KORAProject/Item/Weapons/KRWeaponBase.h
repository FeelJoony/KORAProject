#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayAbilitySpecHandle.h"
#include "KRWeaponBase.generated.h"

struct FKRHeroWeaponData;
struct FKRWeaponCommonData;
class UDataAsset_Weapon;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTargetInteractedDelegate, AActor*, const FHitResult&);

UCLASS()
class KORAPROJECT_API AKRWeaponBase : public AActor
{
	GENERATED_BODY()

public:	
	AKRWeaponBase();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Config")
	TObjectPtr<const UDataAsset_Weapon> WeaponData;

	UFUNCTION(BlueprintCallable)
	void AssignGrantAbilitySpecHandles(const TArray<FGameplayAbilitySpecHandle>& InSpecHandles);

	UFUNCTION(BlueprintPure)
	TArray<FGameplayAbilitySpecHandle> GetGrantedAbilitySpecHandles() const;
	
	UFUNCTION(BlueprintCallable, Category = "Weapon|Config")
	FKRWeaponCommonData GetCommonData() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon|Config")
	bool GetHeroData(FKRHeroWeaponData& OutHeroData) const;


	FOnTargetInteractedDelegate OnWeaponHitTarget;
	FOnTargetInteractedDelegate OnWeaponPulledFromTarget;
	
	void ReportHit(AActor* HitActor, const FHitResult& HitResult);
	void ReportPulled(AActor* HitActor, const FHitResult& HitResult);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

private:
	TArray<FGameplayAbilitySpecHandle> GrantedAbilitySpecHandles;
};
