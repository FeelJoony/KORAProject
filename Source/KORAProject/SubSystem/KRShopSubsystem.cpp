#include "Subsystem/KRShopSubsystem.h"
#include "Subsystem/KRInventorySubsystem.h"
#include "Subsystem/KRDataTablesSubsystem.h"
#include "Item/DataTable/KRBaseItemData.h"
#include "Item/DataTable/KRConsumeItemData.h"
#include "Data/GameDataType.h"
#include "Data/ShopItemData.h"
#include "Data/CacheDataTable.h"
#include "Inventory/KRInventoryItemInstance.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "Inventory/Fragment/InventoryFragment_SellableItem.h"
#include "Inventory/Fragment/InventoryFragment_DisplayUI.h"
#include "GameplayTag/KRShopTag.h"
#include "GameplayTag/KRItemTypeTag.h"
#include "GameplayTag/KRAbilityTag.h"
#include "Math/UnrealMathUtility.h"

DEFINE_LOG_CATEGORY(LogShopSubSystem);

void UKRShopSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Collection.InitializeDependency<UKRInventorySubsystem>();
	Collection.InitializeDependency<UKRDataTablesSubsystem>();
}

void UKRShopSubsystem::GenerateShopStock(FGameplayTag InPoolTag)
{
	if (!PrepareShopGeneration() || !InPoolTag.IsValid())
	{
		return;
	}
	
	TArray<FShopItemData*> AlwaysOnCandidates;
	TArray<FShopItemData*> RotationCandidates;
	
	GatherShopCandidates(InPoolTag, AlwaysOnCandidates, RotationCandidates);
	
	AddAlwaysOnItemsToStock(AlwaysOnCandidates);
	
	const int32 SlotsToFill = MaxShopSlots - CurrentShopInventory.Num();
	if (SlotsToFill > 0 && !RotationCandidates.IsEmpty())
	{
		AddRotationItemsToStock(RotationCandidates, SlotsToFill);
	}
}

TArray<UKRInventoryItemInstance*> UKRShopSubsystem::GetShopStock()
{
	return CurrentShopInventory;
}

bool UKRShopSubsystem::TryBuyItem(APlayerController* InPlayer, UKRInventoryItemInstance* InShopItemInstance, int32 InCount)
{
	if (!InPlayer || !InShopItemInstance || InCount <= 0) return false;

	if (!ShopStockCountMap.Contains(InShopItemInstance)) return false;
	
	int32& CurrentStock = ShopStockCountMap[InShopItemInstance];
	if (CurrentStock < InCount)
	{
		UE_LOG(LogShopSubSystem, Warning, TEXT("Not enough stock. Left: %d"), CurrentStock);
		return false;
	}

	const FGameplayTag& DisplayUITag = KRTAG_ABILITY_ITEM_DISPLAYUI;
	
	const UKRInventoryItemFragment* Frag = InShopItemInstance->FindFragmentByTag(DisplayUITag);
	const UInventoryFragment_DisplayUI* DisplayUI = Cast<UInventoryFragment_DisplayUI>(Frag);
	if (!DisplayUI) return false;

	const int32 TotalCost = DisplayUI->Price * InCount;

	UKRInventorySubsystem* InventorySubsystem = GetGameInstance()->GetSubsystem<UKRInventorySubsystem>();
	if (!InventorySubsystem || !CanAfford(InventorySubsystem, TotalCost)) return false;

	InventorySubsystem->SubtractItem(KRTAG_CURRENCY_PURCHASE_GEARING, TotalCost);
	InventorySubsystem->AddItem(InShopItemInstance->GetItemTag(), InCount);

	CurrentStock -= InCount;
	if (CurrentStock <= 0)
	{
		ShopStockCountMap.Remove(InShopItemInstance);
		CurrentShopInventory.Remove(InShopItemInstance);
		UE_LOG(LogShopSubSystem, Log, TEXT("Item Sold Out!"));
	}

	return true;
}

bool UKRShopSubsystem::TrySellItem(APlayerController* InPlayer, FGameplayTag InItemTag, int32 InCount)
{
	if (!InPlayer || !InItemTag.IsValid() || InCount <= 0) return false;

	UKRInventorySubsystem* InventorySubsystem = GetGameInstance()->GetSubsystem<UKRInventorySubsystem>();
	if (!InventorySubsystem) return false;

	if (InventorySubsystem->GetItemCountByTag(InItemTag) < InCount) return false;

	const FGameplayTag& SellableTag = KRTAG_ABILITY_ITEM_SELLABLE;

	const UKRInventoryItemFragment* Fragment = InventorySubsystem->GetItemFragment(InItemTag, SellableTag);
	const UKRInventoryFragment_SellableItem* SellableFragment = Cast<UKRInventoryFragment_SellableItem>(Fragment);

	if (!SellableFragment) return false;

	const int32 BasePrice = SellableFragment->GetBasePrice();
	const float DiscountedPrice = BasePrice * SellBackMultiplier;
	const int32 SellPrice = Round5(DiscountedPrice);
	
	const int32 TotalPayout = SellPrice * InCount;

	InventorySubsystem->SubtractItem(InItemTag, InCount);
	InventorySubsystem->AddItem(KRTAG_CURRENCY_PURCHASE_GEARING, TotalPayout);

	return true;
}

bool UKRShopSubsystem::CanAfford(UKRInventorySubsystem* InInventory, int32 InTotalCost) const
{
	if (!InInventory) return false;
	return InInventory->GetItemCountByTag(KRTAG_CURRENCY_PURCHASE_GEARING) >= InTotalCost;
}

int32 UKRShopSubsystem::Round5(float InValue) const
{
	return FMath::RoundToInt(InValue / 5.0f) * 5;
}

UKRInventoryItemInstance* UKRShopSubsystem::CreateShopItemInstance(const FKRConsumeItemData* InData)
{
	if (!InData) return nullptr;

	UKRInventoryItemInstance* NewInstance = UKRInventoryItemInstance::CreateItemInstance();
	if (!NewInstance) return nullptr;

	NewInstance->CreateItemDefinition();
	NewInstance->SetItemTag(InData->TypeTag);

	UKRInventoryItemDefinition* Def = NewInstance->GetItemDef();
	if (Def)
	{
		UInventoryFragment_DisplayUI* UIFragment = NewObject<UInventoryFragment_DisplayUI>(Def);
		
		UIFragment->DisplayNameKey = FName(*InData->DisplayName.ToString());
		UIFragment->DescriptionKey = FName(*InData->Description.ToString());
		UIFragment->ItemIcon = InData->ItemIcon;
		UIFragment->Price = InData->BasePrice; 
		
		const FGameplayTag& DisplayUITag = KRTAG_ABILITY_ITEM_DISPLAYUI;
		Def->AddFragment(DisplayUITag, UIFragment); 
	}
	
	return NewInstance;
}

bool UKRShopSubsystem::PrepareShopGeneration()
{
	CurrentShopInventory.Empty();
	ShopStockCountMap.Empty();

	if (!GetGameInstance()->GetSubsystem<UKRDataTablesSubsystem>())
	{
		UE_LOG(LogShopSubSystem, Error, TEXT("DataTablesSubsystem is not valid."));
		return false;
	}

	return true;
}

void UKRShopSubsystem::GatherShopCandidates(FGameplayTag InPoolTag, TArray<FShopItemData*>& OutAlwaysOn, TArray<FShopItemData*>& OutRotation)
{
	UKRDataTablesSubsystem* DataSubsystem = GetGameInstance()->GetSubsystem<UKRDataTablesSubsystem>();

	UCacheDataTable* ShopCacheTable = DataSubsystem ? DataSubsystem->GetTable(EGameDataType::ShopStockList) : nullptr;
	if (!ShopCacheTable || !ShopCacheTable->GetTable()) return;

	const UDataTable* ShopDataTable = ShopCacheTable->GetTable();
	const FString ContextString(TEXT("GatherShopCandidates"));

	TArray<FShopItemData*> AllShopItems;
	ShopDataTable->GetAllRows<FShopItemData>(ContextString, AllShopItems);

	for (FShopItemData* ShopItemData : AllShopItems)
	{
		if (!ShopItemData) continue;
		
		if (ShopItemData->PoolTag.MatchesTag(KRTAG_POOL_ALWAYSON))
		{
			OutAlwaysOn.Add(ShopItemData);
		}
		else if (ShopItemData->PoolTag.MatchesTag(InPoolTag))
		{
			OutRotation.Add(ShopItemData);
		}
	}
}

void UKRShopSubsystem::AddAlwaysOnItemsToStock(const TArray<FShopItemData*>& Candidates)
{
	UKRDataTablesSubsystem* DataSubsystem = GetGameInstance()->GetSubsystem<UKRDataTablesSubsystem>();
	if (!DataSubsystem) return;

	for (const FShopItemData* ShopData : Candidates)
	{
		if (!ShopData) continue;
		
		const FKRConsumeItemData* ConsumeData = DataSubsystem->GetData<FKRConsumeItemData>(EGameDataType::ItemData, ShopData->ItemTag);
		if (!ConsumeData)
		{
			UE_LOG(LogShopSubSystem, Warning, TEXT("ItemTag [%s] found in ShopStockList."), *ShopData->ItemTag.ToString());
			continue;
		}

		if (UKRInventoryItemInstance* NewItem = CreateShopItemInstance(ConsumeData))
		{
			CurrentShopInventory.Add(NewItem);
			ShopStockCountMap.Add(NewItem, ShopData->BaseStock);
		}
	}
}

void UKRShopSubsystem::AddRotationItemsToStock(TArray<FShopItemData*>& Candidates, int32 SlotsToFill)
{
    UKRDataTablesSubsystem* DataSubsystem = GetGameInstance()->GetSubsystem<UKRDataTablesSubsystem>();
    if (!DataSubsystem) return;

    float TotalWeight = 0.f;
    for (const FShopItemData* Candidate : Candidates)
    {
        TotalWeight += Candidate->Weight;
    }
    
    for (int32 i = 0; i < SlotsToFill && !Candidates.IsEmpty(); ++i)
    {
        if (TotalWeight <= 0.f) break;

        const float RandomValue = FMath::FRandRange(0.f, TotalWeight);
        float CurrentSum = 0.f;
        int32 Index = -1;

        for (int32 j = 0; j < Candidates.Num(); ++j)
        {
            CurrentSum += Candidates[j]->Weight;
            if (RandomValue <= CurrentSum)
            {
                Index = j;
                break;
            }
        }

        if (Candidates.IsValidIndex(Index))
        {
            const FShopItemData* ShopData = Candidates[Index];
        	
            TotalWeight -= ShopData->Weight;
        	
            const FKRConsumeItemData* ConsumeData = DataSubsystem->GetData<FKRConsumeItemData>(EGameDataType::ItemData, ShopData->ItemTag);
            if (ConsumeData)
            {
                if (UKRInventoryItemInstance* NewItem = CreateShopItemInstance(ConsumeData))
                {
                    CurrentShopInventory.Add(NewItem);
                    ShopStockCountMap.Add(NewItem, ShopData->BaseStock);
                }
            }
            else
            {
                UE_LOG(LogShopSubSystem, Warning, TEXT("ItemTag [%s] found in ShopStockList."), *ShopData->ItemTag.ToString());
            }
            
            Candidates.RemoveAt(Index);
        }
        else
        {
            break;
        }
    }
}