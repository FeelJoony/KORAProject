#pragma once

#include "CoreMinimal.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "Item/ConsumableTypes.h"               // ✅ 추가: EConsumableEffectType 공용 헤더
#include "InventoryFragment_ConsumableItem.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;
class UKRInventoryItemInstance;
class UKRDataTablesSubsystem;

// 여기서 EConsumableEffectType 정의는 제거됨 (ConsumableTypes.h 로 이동)

// 데이터 테이블에서 읽어온 설정을 캐싱하는 용도
USTRUCT(BlueprintType)
struct FConsumableEffectConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effect")
	TSubclassOf<UGameplayEffect> MainEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effect")
	EConsumableEffectType EffectType = EConsumableEffectType::Instant;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effect")
	float Power = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effect")
	float Duration = 0.f;
};

USTRUCT(BlueprintType)
struct FConsumableCooldownConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cooldown")
	TSubclassOf<UGameplayEffect> CooldownEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cooldown")
	float ExtraCooldown = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cooldown")
	FGameplayTag CooldownTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cooldown")
	bool bIncludeDurationInCooldown = true;
};

UCLASS()
class KORAPROJECT_API UInventoryFragment_ConsumableItem : public UKRInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Consumable")
	int32 ConsumeID = -1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Consumable")
	FConsumableEffectConfig EffectConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Consumable")
	FConsumableCooldownConfig CooldownConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Consumable")
	FGameplayTagContainer InUseTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Consumable")
	FGameplayTagContainer RequiredTags;

	virtual FGameplayTag GetFragmentTag() const override
	{
		return FGameplayTag::RequestGameplayTag("Fragment.Item.Consumable");
	}

	virtual void OnInstanceCreated(UKRInventoryItemInstance* Instance) override;

	/** 실제 소모품 사용(효과 + 쿨다운 적용) */
	UFUNCTION(BlueprintCallable, Category="Consumable")   // ✅ BP에서도 쓰고 싶으면
	bool UseConsumable(UAbilitySystemComponent* ASC);

	/** 쿨다운 중인지 */
	UFUNCTION(BlueprintCallable, Category="Consumable")
	bool IsOnCooldown(UAbilitySystemComponent* ASC) const;

	/** 남은 쿨다운 시간 */
	UFUNCTION(BlueprintCallable, Category="Consumable")
	float GetRemainingCooldown(UAbilitySystemComponent* ASC) const;

	/** Duration(인유즈) 상태인지 */
	UFUNCTION(BlueprintCallable, Category="Consumable")
	bool IsInUse(UAbilitySystemComponent* ASC) const;

protected:
	void LoadFromDataTable(UKRInventoryItemInstance* Instance);
	bool ApplyMainEffect(UAbilitySystemComponent* ASC, float& OutDuration);
	bool ApplyCooldown(UAbilitySystemComponent* ASC, float EffectDuration) const;
};
