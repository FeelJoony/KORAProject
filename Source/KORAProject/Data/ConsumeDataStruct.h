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
		, StackMax(0)
		, CooldownDuration(0.f)
	{
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Key")
	int32 Index;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Effect")
	TSoftClassPtr<UGameplayEffect> MainEffectClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Effect")
	EConsumableEffectType EffectType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Effect")
	float Power;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Effect")
	float Duration;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="StackMax")
	int32 StackMax;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cooldown")
	float CooldownDuration;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cooldown")
	FGameplayTag CooldownTag;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tag")
	FGameplayTagContainer InUseTags;

	virtual uint32 GetKey() const override
	{
		return Index;
	}
};
