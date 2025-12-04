#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "GameplayEffect.h"    
#include "Interface/TableKey.h"
#include "Item/ConsumableTypes.h"          // ✅ 추가: EConsumableEffectType 사용
#include "ConsumeDataStruct.generated.h"

USTRUCT(BlueprintType)
struct KORAPROJECT_API FConsumeDataStruct : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()

	FConsumeDataStruct()
		: Index(-1)
		, MainEffectClass(nullptr)
		, EffectType(EConsumableEffectType::Instant)
		, Power(0.f)
		, Duration(0.f)
		, CooldownEffectClass(nullptr)
		, CooldownDuration(0.f)
		, bIncludeDurationInCooldown(true)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Key")
	int32 Index;

	// 메인 효과
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effect")
	TSoftClassPtr<UGameplayEffect> MainEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effect")
	EConsumableEffectType EffectType;   // Instant / HasDuration / Infinite

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effect")
	float Power;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effect")
	float Duration;

	// 쿨다운
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cooldown")
	TSoftClassPtr<UGameplayEffect> CooldownEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cooldown")
	float CooldownDuration; // “추가 쿨다운” (Duration 이후 기다릴 시간)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cooldown")
	FGameplayTag CooldownTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cooldown")
	bool bIncludeDurationInCooldown;

	// Duration 동안 사용 막는 태그들 (InUse)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tag")
	FGameplayTagContainer InUseTags;

	virtual uint32 GetKey() const override
	{
		return Index;
	}
};
