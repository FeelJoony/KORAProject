#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "GameplayTagContainer.h"
#include "Data/DataAssets/KRWorldEventData.h"
#include "KRGameplayCue_WorldEvent.generated.h"

/**
 * 월드 이벤트 GameplayCue - 환경/상호작용 Cue 처리
 */
UCLASS()
class KORAPROJECT_API UKRGameplayCue_WorldEvent : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

public:
	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WorldEvent")
	TSoftObjectPtr<UKRWorldEventData> WorldEventData;
};
