#include "Item/Object/KRBaseItem.h"

UKRBaseItem::UKRBaseItem()
{
}

void UKRBaseItem::Initialize(int32 InIndex, UDataTable* InTable)
{
	if (!InTable)
	{
		//UE_LOG(LogTemp, Error, TEXT("Initialize 실패 : DataTable이 없습니다"));
		return;
	}

	CachedDataIndex = InIndex;
	SourceDataTable = InTable;

	const FBaseItemData* Data = GetBaseData();
	if (!Data)
	{
		UE_LOG(LogTemp, Error, TEXT("아이템 데이터 로드 실패 Index: %d"), InIndex);
		return;
	}

	// 최소 정보 캐싱
	CachedItemID = Data->ItemID;
	CachedDisplayName = Data->DisplayName;
	CachedDescription = Data->Description;
	CachedIcon = Data->ItemIcon;

	// 기본 스택
	CurrentStackCount = FMath::Clamp(1, 1, Data->StackMax);

	UE_LOG(LogTemp, Log, TEXT("아이템 초기화: %s"), *CachedItemID.ToString());
}

const FBaseItemData* UKRBaseItem::GetBaseData() const
{
	if (!SourceDataTable)
		return nullptr;

	return SourceDataTable->FindRow<FBaseItemData>(
		*FString::FromInt(CachedDataIndex),
		TEXT("BaseItemData Lookup"));
}

/* 스택 관리 */
void UKRBaseItem::AddStack(int32 Amount)
{
	if (const FBaseItemData* Data = GetBaseData())
	{
		CurrentStackCount = FMath::Clamp(CurrentStackCount + Amount, 1, Data->StackMax);
	}
}

bool UKRBaseItem::RemoveStack(int32 Amount)
{
	if (!GetBaseData()) return false;

	if (CurrentStackCount - Amount < 1)
		return false;

	CurrentStackCount -= Amount;
	return true;
}

/* 최대 수량은 DT에서 참조 */
int32 UKRBaseItem::GetMaxStackCount() const
{
	if (const FBaseItemData* Data = GetBaseData())
		return Data->StackMax;

	return 1;
}
