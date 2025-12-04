#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "GameplayTagContainer.h"
#include "KRCombatComponent.generated.h"

class UKRAbilitySystemComponent;
class UKRWeaponInstance;
class UKRRangeWeaponInstance;
class AKRWeaponBase;
class AKRMeleeWeapon;

UENUM(BlueprintType)
enum class EToggleDamageType : uint8
{
	CurrentEquippedWeapon,
	Body
};

UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	RightHand,
	LeftHand,
	AllHands,
	Ranged,
	Tail, // etc... Enemy 쪽에서 필요하다면 넣기 
};

UCLASS()
class KORAPROJECT_API UKRCombatComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UKRCombatComponent(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	TMap<EWeaponSlot, TObjectPtr<AKRWeaponBase>> SlotToWeaponMap;

	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<AKRWeaponBase>> CharacterCarriedWeaponMap;

	UPROPERTY()
	FGameplayTag CurrentEquippedWeaponTag;

	// 현재 장착 중인 WeaponInstance / Actor
	UPROPERTY()
	TObjectPtr<UKRWeaponInstance> CurrentWeaponInstance;

	UPROPERTY()
	TObjectPtr<AKRWeaponBase> CurrentWeaponActor;

	UPROPERTY()
	TObjectPtr<UKRAbilitySystemComponent> ASC;

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> OverlappedActors;

public:
	void RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister,
							   AKRWeaponBase* InWeaponToRegister,
							   EWeaponSlot InSlot,
							   bool bRegisterAsEquippedWeapon);

	// Set Instance / Weapon Actor
	void SetCurrentWeapon(UKRWeaponInstance* InInstance, AKRWeaponBase* InActor);

	AKRWeaponBase* GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const;
	AKRWeaponBase* GetCharacterCurrentEquippedWeapon() const;
	AKRWeaponBase* GetWeaponBySlot(EWeaponSlot InSlot) const;

	void ToggleWeaponCollision(bool bShouldEnable,
							   EToggleDamageType ToggleDamageType,
							   EWeaponSlot WeaponSlot = EWeaponSlot::AllHands);

	void HandleMeleeHit(AActor* HitActor, const FHitResult& Hit);
	bool FireRangedWeapon();
	
	void OnHitTargetActor(AActor* HitActor, const FHitResult& HitResult);
	void OnWeaponPulledFromTargetActor(AActor* InteractedActor, const FHitResult& HitResult);

private:
	void ApplyDamageToTarget(AActor* TargetActor, float BaseDamage, bool bIsCritical, const FHitResult* HitResult);

public:
	UFUNCTION(BlueprintCallable, Category = "KR|Combat")
	UKRWeaponInstance* GetCurrentWeaponInstance() const { return CurrentWeaponInstance; }
};
