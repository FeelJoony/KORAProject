#pragma once

#include "CoreMinimal.h"
#include "ConsumableTypes.generated.h"

UENUM(BlueprintType)
enum class EConsumableEffectType : uint8
{
	Instant       UMETA(DisplayName = "Instant"),       // 즉시 적용 (회복 포션)
	HasDuration   UMETA(DisplayName = "HasDuration"),   // 지속시간 있음 (버프/라이트)
	Infinite      UMETA(DisplayName = "Infinite")       // 무한 지속 (보험/코인)
};
