//#include "SubSystem/KRItemSubsystem.h"
//#include "Item/Object/KRBaseItem.h"
//#include "Data/BaseItemData.h"
//#include "GAS/KRGameplayTags.h"
//#include "Item/Object/KRConsumeItem.h"
//#include "Item/Object/KRWeaponItem.h"
//
//void UKRItemSubsystem::Initialize(FSubsystemCollectionBase& Collection)
//{
//	Super::Initialize(Collection);
//	
//	LoadDataTable(EGameDataType::WeaponItemData, TEXT("WeaponItemData"));
//	LoadDataTable(EGameDataType::ConsumeItemData, TEXT("ConsumeItemData"));
//	LoadDataTable(EGameDataType::MaterialItemData, TEXT("MaterialItemData"));
//}
//
//void UKRItemSubsystem::Deinitialize()
//{
//	ItemDataTables.Empty();
//	Super::Deinitialize();
//}
//
//void UKRItemSubsystem::LoadDataTable(EGameDataType ItemType, const FString& TableName)
//{
//	const FString FullPath = FPaths::Combine(*TablePath, *FString::Printf(TEXT("%s.%s"), *TableName, *TableName));
//	UDataTable* LoadedTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *FullPath));
//
//	if (IsValid(LoadedTable))
//	{
//		ItemDataTables.Add(ItemType, LoadedTable);
//	}
//	else
//	{
//		UE_LOG(LogTemp, Error, TEXT("UKRItemSubsystem: Failed to load DataTable %s"), *FullPath);
//	}
//}
//
//const FBaseItemData* UKRItemSubsystem::GetItemData(EGameDataType ItemType, int32 ItemKey) const
//{
//	if (const TObjectPtr<UDataTable>* FoundTablePtr = ItemDataTables.Find(ItemType))
//	{
//		if (UDataTable* Table = FoundTablePtr->Get())
//		{
//			return Table->FindRow<FBaseItemData>(*FString::FromInt(ItemKey), TEXT("GetItemData"));
//		}
//	}
//	return nullptr;
//}
//
//UKRBaseItem* UKRItemSubsystem::CreateItemInstance(EGameDataType ItemType, int32 ItemKey, int32 Amount)
//{
//	const FBaseItemData* ItemData = GetItemData(ItemType, ItemKey);
//	if (ItemData == nullptr)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("CreateItemInstance: ItemKey %d not found in table type %d"), ItemKey, static_cast<int>(ItemType));
//		return nullptr;
//	}
//	
//	UDataTable* Table = ItemDataTables.FindChecked(ItemType);
//	
//	UClass* ItemObjectClass = UKRBaseItem::StaticClass();
//	if(ItemData->TypeTag.MatchesTag(KRGameplayTags::TAG_ItemType_Equip_Sword) || ItemData->TypeTag.MatchesTag(KRGameplayTags::TAG_ItemType_Equip_Gun))
//	{
//		ItemObjectClass = UKRWeaponItem::StaticClass();
//	}
//	else if (ItemData->TypeTag.MatchesTag(KRGameplayTags::TAG_ItemType_Consume))
//	{
//		ItemObjectClass = UKRConsumeItem::StaticClass(); 
//	}
//	
//	UKRBaseItem* NewItemObject = NewObject<UKRBaseItem>(this, ItemObjectClass);
//	
//	if (NewItemObject)
//	{
//		NewItemObject->Initialize(ItemKey, Table);
//		NewItemObject->AddStack(Amount - 1);
//	}
//
//	return NewItemObject;
//}