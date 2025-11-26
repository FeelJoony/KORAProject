#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "KRShopSubsystem.generated.h"

class UKRInventoryItemInstance;
class UKRInventorySubsystem;
struct FKRConsumeItemData;
struct FShopItemData;

DECLARE_LOG_CATEGORY_EXTERN(LogShopSubSystem, Log, All);

UCLASS()
class KORAPROJECT_API UKRShopSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "KR|Shop")
	void GenerateShopStock(FGameplayTag InPoolTag);

	UFUNCTION(BlueprintCallable, Category = "KR|Shop")
	TArray<UKRInventoryItemInstance*> GetShopStock();

	UFUNCTION(BlueprintCallable, Category = "KR|Shop")
	bool TryBuyItem(APlayerController* InPlayer, UKRInventoryItemInstance* InShopItemInstance, int32 InCount = 1);

	UFUNCTION(BlueprintCallable, Category = "KR|Shop")
	bool TrySellItem(APlayerController* InPlayer, FGameplayTag InItemTag, int32 InCount = 1);

private:
	bool CanAfford(UKRInventorySubsystem* InInventory, int32 InTotalCost) const;

	int32 Round5(float InValue) const;

	UKRInventoryItemInstance* CreateShopItemInstance(const FKRConsumeItemData* InData);
	
	bool PrepareShopGeneration();

	void GatherShopCandidates(FGameplayTag InPoolTag, TArray<FShopItemData*>& OutAlwaysOn, TArray<FShopItemData*>& OutRotation);

	void AddAlwaysOnItemsToStock(const TArray<FShopItemData*>& Candidates);

	void AddRotationItemsToStock(TArray<FShopItemData*>& Candidates, int32 SlotsToFill);
	
private:
	UPROPERTY()
	TArray<TObjectPtr<UKRInventoryItemInstance>> CurrentShopInventory;

	UPROPERTY()
	TMap<TObjectPtr<UKRInventoryItemInstance>, int32> ShopStockCountMap;

	UPROPERTY(EditDefaultsOnly, Category = "KR|Shop")
	float SellBackMultiplier = 0.8f;
	
	UPROPERTY(EditDefaultsOnly, Category = "KR|Shop")
	int32 MaxShopSlots = 6;
};