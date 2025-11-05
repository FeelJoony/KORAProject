#include "Item/Object/KRConsumeItem.h"

UKRConsumeItem::UKRConsumeItem()
{
    // 소모품이므로 초기 스택 1 이상 보장
    CurrentStackCount = FMath::Max(CurrentStackCount, 1);
}

bool UKRConsumeItem::UseItem(AActor* TargetActor)
{
    // 스택 부족
    if (CurrentStackCount <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("소모품 스택이 부족합니다!"));
        return false;
    }

    // 효과 실행 실패하면 소모 안 함
    if (!ExecuteConsumeEffect(TargetActor))
    {
        return false;
    }

    // 효과 실행 성공 → 스택 감소
    RemoveStack(1);

    UE_LOG(LogTemp, Log, TEXT("소모품 사용: %s | 남은 스택: %d"),
        *CachedItemID.ToString(),
        CurrentStackCount
    );

    return true;
}

bool UKRConsumeItem::ExecuteConsumeEffect_Implementation(AActor* TargetActor)
{
    // 기본은 효과 없음 → 실패 처리
    UE_LOG(LogTemp, Warning, TEXT("ConsumeItem 효과 미구현 (아이템 ID: %s)"),
        *CachedItemID.ToString());
    return false;
}
