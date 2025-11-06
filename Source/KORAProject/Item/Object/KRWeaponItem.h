#pragma once

#include "CoreMinimal.h"
#include "Item/Object/KREquipItem.h"
#include "Data/WeaponItemData.h"
#include "KRWeaponItem.generated.h"

UCLASS(Blueprintable)
class KORAPROJECT_API UKRWeaponItem : public UKREquipItem
{
	GENERATED_BODY()

public:
	UKRWeaponItem();

	/* 무기 스탯 초기화 (BaseData Load 이후 수동 호출) */
	UFUNCTION(BlueprintCallable, Category = "스탯")
	void SetupWeaponStats();

protected:

	/* 현재 무기 스탯 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "스탯")
	int32 CurrentATK = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "스탯")
	float CurrentCritChance = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "스탯")
	float CurrentCritMulti = 1.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "스탯")
	float CurrentRange = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "스탯")
	float CurrentAccuracy = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "스탯")
	float CurrentRecoil = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "스탯")
	int32 CurrentCapacity = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "스탯")
	float CurrentAttackSpeed = 1.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "스탯")
	float CurrentReloadTime = 1.f;

	/* 강화 단계 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "강화")
	int32 EnforceLevel = 0;

public:
	/* Getter */
	UFUNCTION(BlueprintPure, Category = "스탯") 
	int32 GetATK() const { return CurrentATK; }

	UFUNCTION(BlueprintPure, Category = "스탯") 
	float GetCritChance() const { return CurrentCritChance; }

	UFUNCTION(BlueprintPure, Category = "스탯") 
	float GetCritMulti() const { return CurrentCritMulti; }

	UFUNCTION(BlueprintPure, Category = "스탯") 
	float GetRange() const { return CurrentRange; }

	UFUNCTION(BlueprintPure, Category = "스탯") 
	float GetAccuracy() const { return CurrentAccuracy; }

	UFUNCTION(BlueprintPure, Category = "스탯") 
	float GetRecoil() const { return CurrentRecoil; }

	UFUNCTION(BlueprintPure, Category = "스탯") 
	int32 GetCapacity() const { return CurrentCapacity; }

	UFUNCTION(BlueprintPure, Category = "스탯") 
	float GetAttackSpeed() const { return CurrentAttackSpeed; }

	UFUNCTION(BlueprintPure, Category = "스탯") 
	float GetReloadTime() const { return CurrentReloadTime; }

	UFUNCTION(BlueprintPure, Category = "강화") 
	int32 GetEnforceLevel() const { return EnforceLevel; }

	// 필요한 데이터 묶음으로 Return 하는 함수도 필요해 보임 너무 개별적으로 하나씩 반환함

	/* 강화 시도 */
	UFUNCTION(BlueprintCallable, Category = "강화")
	bool TryEnforce();

private:
	/* BaseData → WeaponData 캐스팅 */
	const FWeaponItemData* GetWeaponData() const;

	/* 강화 포함 스탯 갱신 */
	void RefreshStats();
};