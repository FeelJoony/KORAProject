#pragma once

#include "CoreMinimal.h"
#include "Item/Object/KRBaseItem.h"
#include "KRConsumeItem.generated.h"

UCLASS(Blueprintable)
class KORAPROJECT_API UKRConsumeItem : public UKRBaseItem
{
    GENERATED_BODY()

public:
    UKRConsumeItem();

    /* 소모품 사용 (true = 효과 실행 성공 & 스택 감소) */
    UFUNCTION(BlueprintCallable, Category = "소모품")
    bool UseItem(AActor* TargetActor);

protected:

    /** 실제 효과 실행 (BP에서 구현 가능)
      * 성공 시 true
      * 실패 시 false
      */
    UFUNCTION(BlueprintNativeEvent, Category = "소모품")
    bool ExecuteConsumeEffect(AActor* TargetActor);
    virtual bool ExecuteConsumeEffect_Implementation(AActor* TargetActor);
};
