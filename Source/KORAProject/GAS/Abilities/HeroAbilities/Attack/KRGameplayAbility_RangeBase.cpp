#include "GAS/Abilities/HeroAbilities/Attack/KRGameplayAbility_RangeBase.h"
#include "Item/Weapons/KRRangeWeapon.h"
#include "Equipment/KREquipmentManagerComponent.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "GAS/Abilities/HeroAbilities/KRGA_LockOn.h"
#include "Characters/KRHeroCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameplayTag/KRCombatTag.h"
#include "GameplayTag/KRItemTypeTag.h"

UKRGameplayAbility_RangeBase::UKRGameplayAbility_RangeBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FireTag = KRTAG_COMBAT_GUN_FIRE;
}

FRotator UKRGameplayAbility_RangeBase::GetFinalAimRotation(float InMaxRange) const
{
	APawn* AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());

	if (!AvatarPawn) return FRotator::ZeroRotator;

	FVector MuzzleLoc = AvatarPawn->GetActorLocation();

	if (AKRRangeWeapon* Weapon = GetRangeWeapon())
	{
		MuzzleLoc = Weapon->GetMuzzleTransform().GetLocation();
	}

	if (AActor* LockedTarget = UKRGA_LockOn::GetLockedTargetFor(AvatarPawn))
	{
		return UKismetMathLibrary::FindLookAtRotation(MuzzleLoc, LockedTarget->GetActorLocation());
	}

	AController* PC = AvatarPawn->GetController();

	if (!PC) return AvatarPawn->GetControlRotation();

	FVector CamLoc; 
	FRotator CamRot;

	PC->GetPlayerViewPoint(CamLoc, CamRot);

	FVector TraceStart = CamLoc;
	FVector TraceEnd = CamLoc + (CamRot.Vector() * InMaxRange);

	FHitResult Hit;

	FCollisionQueryParams Params;

	Params.AddIgnoredActor(AvatarPawn);

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params);

	FVector TargetPoint = bHit ? Hit.Location : TraceEnd;

	return UKismetMathLibrary::FindLookAtRotation(MuzzleLoc, TargetPoint);
}

void UKRGameplayAbility_RangeBase::FireWeaponActor(const FRotator& InAimRotation)
{
	if (AKRRangeWeapon* Weapon = GetRangeWeapon())
	{
		Weapon->FireProjectile(InAimRotation);
	}
}

AKRRangeWeapon* UKRGameplayAbility_RangeBase::GetRangeWeapon() const
{
	if (AKRBaseCharacter* Character = Cast<AKRBaseCharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UKREquipmentManagerComponent* EMC = Character->FindComponentByClass<UKREquipmentManagerComponent>())
		{
			return EMC->GetRangeActorInstance();
		}
	}
	
	return nullptr;
}

UKREquipmentManagerComponent* UKRGameplayAbility_RangeBase::GetEquipmentManager() const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();

	if (!AvatarActor) return nullptr;

	return AvatarActor->FindComponentByClass<UKREquipmentManagerComponent>();
}

const TArray<TObjectPtr<UAnimMontage>>* UKRGameplayAbility_RangeBase::GetMontageArrayFromEntry(const FKRAppliedEquipmentEntry& Entry) const
{
	return nullptr; 
}

UGameplayEffect* UKRGameplayAbility_RangeBase::GetCostGameplayEffect() const
{
	if (AmmoCostEffectClass)
	{
		UE_LOG(LogTemp, Warning, TEXT(">> [CostCheck] Custom Cost Found: %s"), *AmmoCostEffectClass->GetName());
		return AmmoCostEffectClass.GetDefaultObject();
	}
	
	UE_LOG(LogTemp, Error, TEXT(">> [CostCheck] Custom Cost is NULL! Falling back to default."));
	return Super::GetCostGameplayEffect();
}

UAnimMontage* UKRGameplayAbility_RangeBase::GetMontageFromEquipment(int32 MontageIndex) const
{
	UKREquipmentManagerComponent* EquipMgr = GetEquipmentManager();

	if (!EquipMgr) return nullptr;
	
	AKRRangeWeapon* RangeWeapon = EquipMgr->GetRangeActorInstance();

	if (!RangeWeapon) return nullptr;

	FGameplayTag GunTag = RangeWeapon->GetWeaponItemTag();
	
	const FKRAppliedEquipmentEntry& Entry = EquipMgr->FindEntryByTag(GunTag);
	
	if (Entry.IsValid())
	{
		const TArray<TObjectPtr<UAnimMontage>>* Montages = GetMontageArrayFromEntry(Entry);
		if (Montages && Montages->IsValidIndex(MontageIndex))
		{
			return (*Montages)[MontageIndex];
		}
	}

	return nullptr;
}

