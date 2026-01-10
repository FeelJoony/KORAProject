#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "KRWeaponStatsCache.generated.h"

USTRUCT(BlueprintType)
struct KORAPROJECT_API FWeaponStatsCache
{
	GENERATED_BODY()
	
	static constexpr float DefaultAttackPower = 0.f;
	static constexpr float DefaultAttackSpeed = 1.f;
	static constexpr float DefaultRange = 200.f;
	static constexpr float DefaultCritChance = 0.f;
	static constexpr float DefaultCritMulti = 1.5f;
	static constexpr float DefaultCapacity = 0;
	static constexpr float DefaultReloadTime = 0.f;

	FWeaponStatsCache()
		: AttackPower(DefaultAttackPower)
		, AttackSpeed(DefaultAttackSpeed)
		, Range(DefaultRange)
		, CritChance(DefaultCritChance)
		, CritMulti(DefaultCritMulti)
		, Capacity(DefaultCapacity)
		, ReloadTime(DefaultReloadTime)
		, bIsInitialized(false)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Range;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float CritChance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float CritMulti;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Gun")
	float Capacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Gun")
	float ReloadTime;
	
	bool bIsInitialized;
	
	bool IsValid() const
	{
		return bIsInitialized && (
			!FMath::IsNearlyEqual(AttackPower, DefaultAttackPower) ||
			!FMath::IsNearlyEqual(AttackSpeed, DefaultAttackSpeed) ||
			!FMath::IsNearlyEqual(Range, DefaultRange) ||
			!FMath::IsNearlyEqual(CritChance, DefaultCritChance) ||
			!FMath::IsNearlyEqual(CritMulti, DefaultCritMulti) ||
			!FMath::IsNearlyEqual(ReloadTime, DefaultReloadTime) ||
			!FMath::IsNearlyEqual(Capacity, DefaultCapacity)
		);
	}
	
	bool HasAnyModifier() const
	{
		return !FMath::IsNearlyEqual(AttackPower, DefaultAttackPower) ||
				  !FMath::IsNearlyEqual(AttackSpeed, DefaultAttackSpeed) ||
				  !FMath::IsNearlyEqual(CritChance, DefaultCritChance) ||
				  !FMath::IsNearlyEqual(CritMulti, DefaultCritMulti) ||
				  !FMath::IsNearlyEqual(Range, DefaultRange) ||
				  	!FMath::IsNearlyEqual(Capacity, DefaultCapacity);
	}

	void Reset()
	{
		AttackPower = DefaultAttackPower;
		AttackSpeed = DefaultAttackSpeed;
		Range = DefaultRange;
		CritChance = DefaultCritChance;
		CritMulti = DefaultCritMulti;
		Capacity = DefaultCapacity;
		ReloadTime = DefaultReloadTime;
		bIsInitialized = false;
	}

	void MarkInitialized() { bIsInitialized = true; }
};
