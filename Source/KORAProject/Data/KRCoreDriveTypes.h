#pragma once

#include "CoreMinimal.h"
#include "KRCoreDriveTypes.generated.h"

/**
 * 코어드라이브 충전 설정
 */
USTRUCT(BlueprintType)
struct FKRCoreDriveChargeConfig
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "Charge", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HitChargeMultiplier = 0.1f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Charge", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float PerfectGuardChargeAmount = 20.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Charge", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float KillChargeAmount = 30.0f;
};

/**
 * 코어드라이브 버스트 설정
 */
USTRUCT(BlueprintType)
struct FKRCoreDriveBurstConfig
{
	GENERATED_BODY()

	/** 버스트 사용에 필요한 퍼센트 (0~1) */
	UPROPERTY(EditDefaultsOnly, Category = "Burst", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float BurstRequiredPercent = 1.0f;

	/** 버스트 사용 시 스태미나 전체 소모 여부 */
	UPROPERTY(EditDefaultsOnly, Category = "Burst")
	bool bDrainsAllStamina = true;
};

// 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCoreDriveFullSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCoreDriveChangedSignature, float, CurrentCoreDrive, float, MaxCoreDrive);
