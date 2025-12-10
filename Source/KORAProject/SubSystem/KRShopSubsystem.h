#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "Data/ShopItemDataStruct.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "KRShopSubsystem.generated.h"

class UKRInventoryItemInstance;
class UKRInventorySubsystem;
struct FKRUIMessage_Confirm;

DECLARE_LOG_CATEGORY_EXTERN(LogShopSubSystem, Log, All);

UCLASS()
class KORAPROJECT_API UKRShopSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "KR|Shop")
	const TArray<UKRInventoryItemInstance*>& GetShopStock() const { return CurrentShopInventory; }
	UFUNCTION(BlueprintCallable, Category = "KR|Shop")
	int32 GetStockCountByItemTag(FGameplayTag ItemTag) const;

private:
	void GenerateShopStock();
	void LoadShopDataCache();

	bool TryBuyItem(FGameplayTag ItemTag, int32 InCount = 1);
	bool TrySellItem(FGameplayTag ItemTag, int32 InCount = 1);

	bool CanAfford(UKRInventorySubsystem* Inventory, int32 TotalCost) const;
	int32 Round5(float Value) const;
	
	UKRInventoryItemInstance* CreateShopItemInstance(FGameplayTag ItemTag);
	void InitializeShopItemFragments(UKRInventoryItemInstance* Instance);
	
	bool PrepareShopGeneration();

	void GatherShopCandidates(TArray<FShopItemDataStruct*>& OutAlwaysOn, TArray<FShopItemDataStruct*>& OutRotation);
	void AddAlwaysOnItemsToStock(const TArray<FShopItemDataStruct*>& Candidates);
	void AddRotationItemsToStock(TArray<FShopItemDataStruct*>& Candidates, int32 SlotsToFill);
	
	UKRInventoryItemInstance* FindShopItemInstanceByTag(FGameplayTag ItemTag) const;

	void OnConfirmMessage(FGameplayTag MessageTag, const FKRUIMessage_Confirm& Payload);
	
private:
	// Convert to map if item >= 50
	UPROPERTY()
	TArray<TObjectPtr<UKRInventoryItemInstance>> CurrentShopInventory;

	UPROPERTY()
	TMap<FGameplayTag, int32> ShopStockCountMap;

	UPROPERTY()
	TArray<FShopItemDataStruct> CachedShopData;

	UPROPERTY(EditDefaultsOnly, Category = "KR|Shop")
	float SellBackMultiplier = 0.8f;
	
	UPROPERTY(EditDefaultsOnly, Category = "KR|Shop")
	int32 MaxShopSlots = 6;

	FGameplayMessageListenerHandle ConfirmMessageHandle;
};