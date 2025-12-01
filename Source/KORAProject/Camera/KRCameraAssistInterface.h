#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "KRCameraAssistInterface.generated.h"


UINTERFACE(BlueprintType)
class UKRCameraAssistInterface : public UInterface
{
	GENERATED_BODY()
};

class KORAPROJECT_API IKRCameraAssistInterface
{
	GENERATED_BODY()

public:
	// 카메라 충돌 처리 시 무시해야 할 액터 목록 반환
	virtual void GetIgnoredActorsForCameraPenetration(TArray<const AActor*>& OutActorsAllowPenetration) const { }

	// 카메라 충돌 방지를 위한 대체 타겟 반환
	virtual TOptional<AActor*> GetCameraPreventPenetrationTarget() const { return TOptional<AActor*>(); }

	// 카메라가 타겟(캐릭터) 내부로 뚫고 들어왔을 때 호출
	virtual void OnCameraPenetratingTarget() { }
};
