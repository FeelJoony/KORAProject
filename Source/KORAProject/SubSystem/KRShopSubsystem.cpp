#include "Subsystem/KRShopSubsystem.h"
#include "Subsystem/KRInventorySubsystem.h"
#include "Subsystem/KRDataTablesSubsystem.h"
#include "Data/GameDataType.h"
#include "Data/ShopItemDataStruct.h"
#include "Data/CacheDataTable.h"
#include "Inventory/KRInventoryItemInstance.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "Inventory/Fragment/InventoryFragment_DisplayUI.h"
#include "Inventory/Fragment/InventoryFragment_SellableItem.h"
#include "GameplayTag/KRShopTag.h"
#include "GameplayTag/KRAbilityTag.h"
#include "GameplayTag/KRUITag.h"
#include "Player/KRPlayerState.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"

class AKRPlayerState;
DEFINE_LOG_CATEGORY(LogShopSubSystem);

void UKRShopSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Collection.InitializeDependency<UKRInventorySubsystem>();
	Collection.InitializeDependency<UKRDataTablesSubsystem>();

	LoadShopDataCache();

	UGameplayMessageSubsystem::Get(GetGameInstance()).RegisterListener(
		KRTAG_UI_MESSAGE_CONFIRM,
		this,
		&UKRShopSubsystem::OnConfirmMessage
	);
	
	GenerateShopStock();
}

void UKRShopSubsystem::Deinitialize()
{
	UGameplayMessageSubsystem::Get(GetGameInstance()).UnregisterListener(ConfirmMessageHandle);
	Super::Deinitialize();
}

int32 UKRShopSubsystem::GetStockCountByItemTag(FGameplayTag ItemTag) const
{
	if (!ItemTag.IsValid()) return 0;
	if (const int32* Found = ShopStockCountMap.Find(ItemTag))
	{
		return *Found;
	}

	return 0;
}

UKRCurrencyComponent* UKRShopSubsystem::GetCurrencyComponent() const
{
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			if (AKRPlayerState* KRPS = PC->GetPlayerState<AKRPlayerState>())
			{
				return KRPS->FindComponentByClass<UKRCurrencyComponent>();
			}
		}
	}
	
	return nullptr;
}

void UKRShopSubsystem::GenerateShopStock()
{
	if (!PrepareShopGeneration())
	{
		return;
	}
    
	TArray<FShopItemDataStruct*> AlwaysOnCandidates;
	TArray<FShopItemDataStruct*> RotationCandidates;
	
	GatherShopCandidates(AlwaysOnCandidates, RotationCandidates);
	
	AddAlwaysOnItemsToStock(AlwaysOnCandidates);
    
	const int32 SlotsToFill = MaxShopSlots - CurrentShopInventory.Num();
	if (SlotsToFill > 0 && !RotationCandidates.IsEmpty())
	{
		AddRotationItemsToStock(RotationCandidates, SlotsToFill);
	}
}
void UKRShopSubsystem::LoadShopDataCache()
{
	UKRDataTablesSubsystem* DataSubsystem = GetGameInstance()->GetSubsystem<UKRDataTablesSubsystem>();
	UCacheDataTable* ShopCacheTable = DataSubsystem ? DataSubsystem->GetTable(FShopItemDataStruct::StaticStruct()) : nullptr;
	if (!ShopCacheTable || !ShopCacheTable->GetTable()) return;

	const UDataTable* ShopDataTable = ShopCacheTable->GetTable();
	const FString ContextString(TEXT("LoadShopDataCache"));
	CachedShopData.Reset();

	TArray<FShopItemDataStruct*> TempRows;
	ShopDataTable->GetAllRows(ContextString, TempRows);

	for (const FShopItemDataStruct* Row : TempRows)
	{
		if (Row)
		{
			CachedShopData.Add(*Row);
		}
	}
}

bool UKRShopSubsystem::TryBuyItem(FGameplayTag ItemTag, int32 InCount)
{
	if (!ShopStockCountMap.Contains(ItemTag)) return false;
	
	int32& CurrentStock = ShopStockCountMap[ItemTag];
	if (CurrentStock < InCount)
	{
		UE_LOG(LogShopSubSystem, Warning, TEXT("Not enough stock. Left: %d"), CurrentStock);
		return false;
	}

	UKRInventorySubsystem* InventorySubsystem = GetGameInstance()->GetSubsystem<UKRInventorySubsystem>();
	if (!InventorySubsystem) return false;

	UKRCurrencyComponent* CurrencyComp = GetCurrencyComponent();
	if (!CurrencyComp) return false;
	
	UKRInventoryItemInstance* ShopItem = FindShopItemInstanceByTag(ItemTag);
	if (!ShopItem) return false;

	const FGameplayTag& DisplayUITag = KRTAG_ABILITY_ITEM_DISPLAYUI;
	const UKRInventoryItemFragment* Frag = ShopItem->FindFragmentByTag(DisplayUITag);
	const UInventoryFragment_DisplayUI* DisplayUI = Cast<UInventoryFragment_DisplayUI>(Frag);
	if (!DisplayUI) return false;

	const int32 TotalCost = DisplayUI->Price * InCount;
	if (!CurrencyComp->CanAfford(KRTAG_CURRENCY_PURCHASE_GEARING, TotalCost))
	{
		UE_LOG(LogShopSubSystem, Warning, TEXT("TryBuyItem Failed: Not enough currency."));
		return false;
	}

	CurrentStock -= InCount;
	if (CurrentStock <= 0)
	{
		ShopStockCountMap.Remove(ItemTag);
		CurrentShopInventory.Remove(ShopItem);
		UE_LOG(LogShopSubSystem, Log, TEXT("Item Sold Out!"));
	}

	InventorySubsystem->AddItem(ItemTag, InCount);
	CurrencyComp->SpendCurrency(KRTAG_CURRENCY_PURCHASE_GEARING, TotalCost);
	
	return true;
}

bool UKRShopSubsystem::TrySellItem(FGameplayTag InItemTag, int32 InCount)
{
	if (!InItemTag.IsValid() || InCount <= 0) return false;

	UKRInventorySubsystem* InventorySubsystem = GetGameInstance()->GetSubsystem<UKRInventorySubsystem>();
	if (!InventorySubsystem) return false;

	UKRCurrencyComponent* CurrencyComp = GetCurrencyComponent();
	if (!CurrencyComp) return false;

	if (InventorySubsystem->GetItemCountByTag(InItemTag) < InCount) return false;

	const FGameplayTag& SellableTag = KRTAG_ABILITY_ITEM_SELLABLE;

	const UKRInventoryItemFragment* Fragment = InventorySubsystem->GetItemFragment(InItemTag, SellableTag);
	const UKRInventoryFragment_SellableItem* SellableFragment = Cast<UKRInventoryFragment_SellableItem>(Fragment);

	if (!SellableFragment) return false;

	const int32 SellPrice = Round5(SellableFragment->GetSellPrice());
	
	const int32 TotalPayout = SellPrice * InCount;
	
	InventorySubsystem->SubtractItem(InItemTag, InCount);
	
	CurrencyComp->AddCurrency(KRTAG_CURRENCY_PURCHASE_GEARING, TotalPayout);

	return true;
}


bool UKRShopSubsystem::CanAfford(UKRInventorySubsystem* InInventory, int32 InTotalCost) const
{
	UKRCurrencyComponent* CurrencyComp = GetCurrencyComponent();
	if (!CurrencyComp) return false;

	return CurrencyComp->CanAfford(KRTAG_CURRENCY_PURCHASE_GEARING, InTotalCost);
}

int32 UKRShopSubsystem::Round5(float InValue) const
{
	return FMath::RoundToInt(InValue / 5.0f) * 5;
}

UKRInventoryItemInstance* UKRShopSubsystem::CreateShopItemInstance(FGameplayTag ItemTag)
{
	if (!ItemTag.IsValid()) return nullptr;

	UKRInventoryItemInstance* NewInstance = UKRInventoryItemInstance::CreateItemInstance();
	if (!NewInstance) return nullptr;

	NewInstance->SetOwnerContext(GetGameInstance());
	NewInstance->SetItemTag(ItemTag);
	NewInstance->CreateItemDefinition();

	InitializeShopItemFragments(NewInstance);

	auto* DisplayUI = Cast<UInventoryFragment_DisplayUI>(NewInstance->FindFragmentByTag(KRTAG_ABILITY_ITEM_DISPLAYUI));
	if (!DisplayUI || DisplayUI->Price <= 0)
	{
		UE_LOG(LogShopSubSystem, Warning, TEXT("Item %s has invalid price"), *ItemTag.ToString());
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

void UKRShopSubsystem::InitializeShopItemFragments(UKRInventoryItemInstance* Instance)
{
	UKRInventoryItemDefinition* Def = Instance->GetItemDef();
	auto* DisplayUI = NewObject<UInventoryFragment_DisplayUI>(Instance);
	Def->AddFragment(KRTAG_ABILITY_ITEM_DISPLAYUI, DisplayUI);
	DisplayUI->OnInstanceCreated(Instance);

	auto* Sellable = NewObject<UKRInventoryFragment_SellableItem>(Instance);
	Def->AddFragment(KRTAG_ABILITY_ITEM_SELLABLE, Sellable);
	Sellable->OnInstanceCreated(Instance);
}

void UKRShopSubsystem::GatherShopCandidates(TArray<FShopItemDataStruct*>& OutAlwaysOn, TArray<FShopItemDataStruct*>& OutRotation)
{
	for (FShopItemDataStruct& ShopItemData : CachedShopData)
	{
		if (!ShopItemData.ItemTag.IsValid()) continue;

		if (ShopItemData.PoolTag.MatchesTag(KRTAG_POOL_ALWAYSON))
		{
			OutAlwaysOn.Add(&ShopItemData);
			continue; 
		}

		OutRotation.Add(&ShopItemData);
	}
}

void UKRShopSubsystem::AddAlwaysOnItemsToStock(const TArray<FShopItemDataStruct*>& Candidates)
{
	for (const FShopItemDataStruct* ShopData : Candidates)
	{
		if (!ShopData || !ShopData->ItemTag.IsValid()) continue;

		if (UKRInventoryItemInstance* NewItem = CreateShopItemInstance(ShopData->ItemTag))
		{
			CurrentShopInventory.Add(NewItem);
			ShopStockCountMap.Add(ShopData->ItemTag, ShopData->StockBase);
		}
	}
}

void UKRShopSubsystem::AddRotationItemsToStock(TArray<FShopItemDataStruct*>& Candidates, int32 SlotsToFill)
{
    float TotalWeight = 0.f;
    for (const FShopItemDataStruct* Candidate : Candidates)
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
            const FShopItemDataStruct* ShopData = Candidates[Index];
        	
            TotalWeight -= ShopData->Weight;
        	
			if (UKRInventoryItemInstance* NewItem = CreateShopItemInstance(ShopData->ItemTag))
			{
				CurrentShopInventory.Add(NewItem);
				ShopStockCountMap.Add(ShopData->ItemTag, ShopData->StockBase);
			}
            
            Candidates.RemoveAt(Index);
        }
        else
        {
            break;
        }
    }
}

UKRInventoryItemInstance* UKRShopSubsystem::FindShopItemInstanceByTag(FGameplayTag ItemTag) const
{
	for (UKRInventoryItemInstance* ItemInstance : CurrentShopInventory)
	{
		if (ItemInstance && ItemInstance->GetItemTag() == ItemTag)
		{
			return ItemInstance;
		}
	}
	return nullptr;
}

void UKRShopSubsystem::OnConfirmMessage(FGameplayTag MessageTag, const FKRUIMessage_Confirm& Payload)
{
	if (Payload.Result != EConfirmResult::Yes)
	{
		return;
	}

    UWorld* World = GetWorld();
    if (!World) return;

    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC) return;

	if (Payload.Context == EConfirmContext::ShopBuy)
	{
		TryBuyItem(Payload.ItemTag, Payload.Quantity);
	}
	else if (Payload.Context == EConfirmContext::ShopSell)
	{
		TrySellItem(Payload.ItemTag, Payload.Quantity);
	}
}