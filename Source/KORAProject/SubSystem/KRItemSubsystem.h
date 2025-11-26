#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "KRItemSubsystem.generated.h"

class UKRInventoryItemDefinition;
class UKRInventoryItemInstance;
class UKRInventorySubsystem;

UCLASS()
class KORAPROJECT_API UKRItemSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	/*-------------------------------------
		Item 생성 공통 API
	-------------------------------------*/

	// 아이템 태그로 생성 (Generic)
	UFUNCTION(BlueprintCallable, Category="ItemSubsystem")
	UKRInventoryItemInstance* CreateItemByTag(FGameplayTag ItemTag);

	// 아이템 정의 + 인스턴스 생성 후 인벤토리에 등록
	UFUNCTION(BlueprintCallable, Category="ItemSubsystem")
	UKRInventoryItemInstance* CreateAndAddToInventory(FGameplayTag ItemTag, int32 StackCount = 1);

public:
	/*-------------------------------------
		무기 생성을 위한 API
	-------------------------------------*/
	UFUNCTION(BlueprintCallable, Category="ItemSubsystem|Weapon")
	UKRInventoryItemInstance* CreateWeaponAndAddToInventory(FName WeaponID);

	// 무기 정의 생성
	UKRInventoryItemDefinition* CreateWeaponDefinition(struct FKRWeaponItemData& WeaponData);

private:
	/*-------------------------------------
		내부 유틸 함수
	-------------------------------------*/
	UKRInventoryItemDefinition* CreateDefinitionInternal(FGameplayTag ItemTag);
	UKRInventoryItemInstance* CreateInstanceInternal(UKRInventoryItemDefinition* Definition);

	// DT Helper
	template<typename T>
	T* GetRowFromDT(const FString& Path, const FName& RowName) const;

private:
	UPROPERTY()
	TObjectPtr<UKRInventorySubsystem> InventorySubsystem;
};
