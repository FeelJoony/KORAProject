// InventoryFragment_ConsumableItem.h

#pragma once

#include "CoreMinimal.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "GameplayTagContainer.h"
#include "Item/ConsumableTypes.h"
#include "InventoryFragment_ConsumableItem.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;
class UKRInventoryItemInstance;

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

	/** 버프/라이트용 InUse 태그 (상태 확인용) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Consumable")
	FGameplayTagContainer InUseTags;

	virtual FGameplayTag GetFragmentTag() const override
	{
		return FGameplayTag::RequestGameplayTag("Fragment.Item.Consumable");
	}

	virtual void OnInstanceCreated(UKRInventoryItemInstance* Instance) override;

	UFUNCTION(BlueprintCallable, Category="Consumable")
	bool UseConsumable(UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category="Consumable")
	bool IsOnCooldown(UAbilitySystemComponent* ASC) const;

	UFUNCTION(BlueprintCallable, Category="Consumable")
	float GetRemainingCooldown(UAbilitySystemComponent* ASC) const;

	UFUNCTION(BlueprintCallable, Category="Consumable")
	bool IsInUse(UAbilitySystemComponent* ASC) const;

protected:
	void LoadFromDataTable(UKRInventoryItemInstance* Instance);
	bool ApplyMainEffect(UAbilitySystemComponent* ASC, float& OutDuration);
	bool ApplyCooldown(UAbilitySystemComponent* ASC, float EffectDuration) const;
};
