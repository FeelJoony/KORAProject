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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effect")
	TSoftClassPtr<UGameplayEffect> MainEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effect")
	EConsumableEffectType EffectType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effect")
	float Power;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effect")
	float Duration;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cooldown")
	TSoftClassPtr<UGameplayEffect> CooldownEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cooldown")
	float CooldownDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cooldown")
	FGameplayTag CooldownTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cooldown")
	bool bIncludeDurationInCooldown;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tag")
	FGameplayTagContainer InUseTags;

	virtual uint32 GetKey() const override
	{
		return Index;
	}
};
