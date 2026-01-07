#include "Inventory/Fragment/InventoryFragment_EnhanceableItem.h"
#include "Inventory/KRInventoryItemInstance.h"
#include "SubSystem/KRDataTablesSubsystem.h"

FGameplayTag UInventoryFragment_EnhanceableItem::GetStaticFragmentTag()
{
	// 프로젝트 태그에 맞게 바꿔줘
	static FGameplayTag Tag = FGameplayTag::RequestGameplayTag(
		TEXT("Fragment.Item.Enhanceable")
	);
	return Tag;
}

void UInventoryFragment_EnhanceableItem::OnInstanceCreated(UKRInventoryItemInstance* Instance)
{
	Super::OnInstanceCreated(Instance);
	
	if (!Instance) { return; }

	WeaponTypeTag = Instance->GetItemTag();
	
	if (!WeaponTypeTag.IsValid()) { return; }
	
	UObject* ContextObj = Instance->GetOwnerContext();
	if (!ContextObj) { return; }
	
	UGameInstance* GI = Cast<UGameInstance>(ContextObj);
	if (!GI) { return; }
	
	auto* DT = UKRDataTablesSubsystem::GetSafe(this);
	if (!DT) { return; }

	FWeaponEnhanceDataStruct* Row = DT->GetData<FWeaponEnhanceDataStruct>(WeaponTypeTag);
	if (!Row) { return; }

}
