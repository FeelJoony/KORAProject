#include "KRItemSubsystem.h"
#include "KRInventorySubsystem.h"
#include "KRInventoryItemDefinition.h"
#include "KRInventoryItemInstance.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameInstance.h"

#include "Data/Weapon/KRWeaponItemData.h"
#include "Data/ItemDataStruct.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(KRItemSubsystem)

void UKRItemSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 인벤토리 참조 저장
	InventorySubsystem = GetGameInstance()->GetSubsystem<UKRInventorySubsystem>();
}

void UKRItemSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

/*===========================================================
	Generic Item Create
===========================================================*/
UKRInventoryItemInstance* UKRItemSubsystem::CreateItemByTag(FGameplayTag ItemTag)
{
	if (!ItemTag.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[ItemSubsystem] Invalid ItemTag"));
		return nullptr;
	}

	// 1. ItemDefinition 생성
	UKRInventoryItemDefinition* NewDef = CreateDefinitionInternal(ItemTag);
	if (!NewDef)
	{
		UE_LOG(LogTemp, Error, TEXT("[ItemSubsystem] CreateDefinition failed: %s"), *ItemTag.ToString());
		return nullptr;
	}

	// 2. ItemInstance 생성
	return CreateInstanceInternal(NewDef);
}

UKRInventoryItemInstance* UKRItemSubsystem::CreateAndAddToInventory(FGameplayTag ItemTag, int32 StackCount)
{
	if (!InventorySubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("[ItemSubsystem] InventorySubsystem invalid!"));
		return nullptr;
	}
	
	return InventorySubsystem->AddItem(ItemTag, StackCount);
}

/*===========================================================
	Weapon Create
===========================================================*/
UKRInventoryItemInstance* UKRItemSubsystem::CreateWeaponAndAddToInventory(FName WeaponID)
{
	// 1. Weapon DT 로드
	FKRWeaponItemData* WeaponData = GetRowFromDT<FKRWeaponItemData>(
		TEXT("/Game/Data/DT_Weapons.DT_Weapons"),
		WeaponID
	);
	if (!WeaponData)
	{
		UE_LOG(LogTemp, Error, TEXT("[ItemSubsystem] WeaponData not found: %s"), *WeaponID.ToString());
		return nullptr;
	}

	// 2. WeaponDefinition 로드
	FString DefPath = FString::Printf(TEXT("/Game/Data/Weapons/WDA_%s.WDA_%s"), 
		*WeaponID.ToString(), *WeaponID.ToString());

	UDA_WeaponDefinition* WeaponDef = LoadObject<UDA_WeaponDefinition>(nullptr, *DefPath);
	if (!WeaponDef)
	{
		UE_LOG(LogTemp, Error, TEXT("[ItemSubsystem] WeaponDefinition Not Found: %s"), *DefPath);
		return nullptr;
	}

	// 3. InventoryItemDefinition 생성
	UKRInventoryItemDefinition* ItemDef = WeaponDef->CreateInventoryItemDefinition(*WeaponData);
	if (!ItemDef)
	{
		return nullptr;
	}

	// 4. 인스턴스 생성
	UKRInventoryItemInstance* ItemInst = CreateInstanceInternal(ItemDef);

	// 5. 인벤토리에 추가
	return InventorySubsystem->AddItem(ItemDef->GetItemTag(), 1);
}

/*===========================================================
	Internal Create Functions
===========================================================*/
UKRInventoryItemDefinition* UKRItemSubsystem::CreateDefinitionInternal(FGameplayTag ItemTag)
{
	// DataTablesSubsystem에서 ItemData 가져오는 방식과 연결 가능
	// 여기서는 최소 기능만 구현
	UKRInventoryItemDefinition* NewDef = UKRInventoryItemDefinition::CreateItemDefinition();
	if (!NewDef)
	{
		return nullptr;
	}

	// ItemTag 적용
	NewDef->SetItemTag(ItemTag);
	return NewDef;
}

UKRInventoryItemInstance* UKRItemSubsystem::CreateInstanceInternal(UKRInventoryItemDefinition* Definition)
{
	UKRInventoryItemInstance* Inst = NewObject<UKRInventoryItemInstance>(this);
	Inst->SetItemDef(Definition);
	return Inst;
}

/*===========================================================
	DataTable Utility
===========================================================*/
template<typename T>
T* UKRItemSu*
