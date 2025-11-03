#include "Item/KRBaseItem.h"

UKRBaseItem::UKRBaseItem()
{
	MaxStackSize = 1;
	Amount = 1;
}

UWorld* UKRBaseItem::GetWorld() const
{
	if (const UObject* MyOuter = GetOuter())
	{
		return MyOuter->GetWorld();
	}
	return nullptr;
}

void UKRBaseItem::SetAmount(int32 NewAmount)
{
	Amount = FMath::Clamp(NewAmount, 0, MaxStackSize);
}