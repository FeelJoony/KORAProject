#include "Inventory/Fragment/InventoryFragment_ConsumableItem.h"

#include "GAS/AttributeSets/KRPlayerAttributeSet.h"
#include "GAS/KRAbilitySystemComponent.h"

#include "Inventory/KRInventoryItemInstance.h"
#include "Subsystem/KRDataTablesSubsystem.h"
#include "Data/ItemDataStruct.h"
#include "Data/ConsumeDataStruct.h"

#include "GameplayTag/KRSetByCallerTag.h"

void UInventoryFragment_ConsumableItem::OnInstanceCreated(UKRInventoryItemInstance* Instance)
{
	Super::OnInstanceCreated(Instance);

	ConsumeID      = -1;
	EffectConfig   = FConsumableEffectConfig();
	CooldownConfig = FConsumableCooldownConfig();
	InUseTags.Reset();

	if (!Instance)
	{
		return;
	}

	LoadFromDataTable(Instance);
}

void UInventoryFragment_ConsumableItem::LoadFromDataTable(UKRInventoryItemInstance* Instance)
{
	UObject* ContextObj = Instance->GetOwnerContext();
	if (!ContextObj)
	{
		return;
	}

	UGameInstance* GI = Cast<UGameInstance>(ContextObj);
	if (!GI)
	{
		return;
	}

	UKRDataTablesSubsystem* DT = GI->GetSubsystem<UKRDataTablesSubsystem>();
	if (!DT)
	{
		return;
	}

	// 1) 아이템 데이터에서 ConsumeID 꺼내오기
	const FGameplayTag ItemTag = Instance->GetItemTag();
	FItemDataStruct* ItemRow = DT->GetData<FItemDataStruct>(EGameDataType::ItemData, ItemTag);
	if (!ItemRow)
	{
		return;
	}

	ConsumeID = ItemRow->ConsumeID;
	if (ConsumeID < 0)
	{
		return;
	}

	// 2) ConsumeData 에서 실제 효과 정보 읽기
	FConsumeDataStruct* ConsumeRow =
		DT->GetData<FConsumeDataStruct>(EGameDataType::ConsumeData, ConsumeID);

	if (!ConsumeRow)
	{
		return;
	}

	EffectConfig.MainEffectClass = ConsumeRow->MainEffectClass.LoadSynchronous();
	EffectConfig.EffectType      = ConsumeRow->EffectType;
	EffectConfig.Power           = ConsumeRow->Power;
	EffectConfig.Duration        = ConsumeRow->Duration;

	CooldownConfig.CooldownEffectClass      = ConsumeRow->CooldownEffectClass.LoadSynchronous();
	CooldownConfig.ExtraCooldown            = ConsumeRow->CooldownDuration;
	CooldownConfig.CooldownTag              = ConsumeRow->CooldownTag;
	CooldownConfig.bIncludeDurationInCooldown = ConsumeRow->bIncludeDurationInCooldown;

	InUseTags = ConsumeRow->InUseTags;
}

bool UInventoryFragment_ConsumableItem::UseConsumable(UAbilitySystemComponent* ASC)
{
	if (!ASC)
	{
		return false;
	}

	// 1) 쿨다운 체크
	if (IsOnCooldown(ASC))
	{
		const float Remain = GetRemainingCooldown(ASC);

		return false;
	}

	// 2) Duration 동안 재사용 방지 (버프형 등)
	if (IsInUse(ASC))
	{
		return false;
	}

	// 3) 메인 효과 적용
	float EffectDuration = 0.f;
	if (!ApplyMainEffect(ASC, EffectDuration))
	{
		return false;
	}

	// 4) 쿨다운 적용
	ApplyCooldown(ASC, EffectDuration);

	return true;
}



bool UInventoryFragment_ConsumableItem::ApplyMainEffect(UAbilitySystemComponent* ASC, float& OutDuration)
{
	OutDuration = 0.f;

	if (!ASC || !EffectConfig.MainEffectClass)
	{
		return false;
	}

	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle =
		ASC->MakeOutgoingSpec(EffectConfig.MainEffectClass, 1.0f, Context);

	if (!SpecHandle.IsValid())
	{
		return false;
	}

	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
	if (!Spec)
	{
		return false;
	}

	// --- SetByCaller 값 세팅 ---
	if (EffectConfig.Power != 0.f)
	{
		Spec->SetSetByCallerMagnitude(KRTAG_SETBYCALLER_CONSUME_POWER, EffectConfig.Power);
	}

	if (EffectConfig.Duration > 0.f)
	{
		Spec->SetSetByCallerMagnitude(KRTAG_SETBYCALLER_CONSUME_DURATION, EffectConfig.Duration);
	}

	// 타입별 처리
	if (EffectConfig.EffectType == EConsumableEffectType::Instant)
	{
		// ✔ 그냥 Apply만 하고 성공 처리
		ASC->ApplyGameplayEffectSpecToSelf(*Spec);

		OutDuration = 0.f;
		return true;
	}
	else
	{
		// HasDuration / Infinite 는 핸들 유효성으로 체크
		FActiveGameplayEffectHandle Handle =
			ASC->ApplyGameplayEffectSpecToSelf(*Spec);

		const bool bSuccess = Handle.IsValid();
		OutDuration = EffectConfig.Duration;

		return bSuccess;
	}
}


bool UInventoryFragment_ConsumableItem::ApplyCooldown(UAbilitySystemComponent* ASC, float EffectDuration) const
{
	if (!ASC || !CooldownConfig.CooldownEffectClass)
	{
		return false;
	}

	float FinalCooldown = CooldownConfig.ExtraCooldown;
	if (CooldownConfig.bIncludeDurationInCooldown)
	{
		FinalCooldown += EffectDuration;
	}

	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle =
		ASC->MakeOutgoingSpec(CooldownConfig.CooldownEffectClass, 1.0f, Context);

	if (!SpecHandle.IsValid())
	{
		return false;
	}

	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
	if (!Spec)
	{
		return false;
	}

	// ① 쿨다운 시간 SetByCaller
	Spec->SetSetByCallerMagnitude(KRTAG_SETBYCALLER_CONSUME_COOLDOWN, FinalCooldown);

	// ② 이 아이템 전용 쿨다운 태그를 GE에 동적으로 부여
	if (CooldownConfig.CooldownTag.IsValid())
	{
		Spec->DynamicGrantedTags.AddTag(CooldownConfig.CooldownTag);
	}

	const FActiveGameplayEffectHandle Handle =
		ASC->ApplyGameplayEffectSpecToSelf(*Spec);

	return Handle.IsValid();
}


// ---------------- 상태 체크 ----------------

bool UInventoryFragment_ConsumableItem::IsOnCooldown(UAbilitySystemComponent* ASC) const
{
	if (!ASC || !CooldownConfig.CooldownTag.IsValid())
	{
		return false;
	}

	return ASC->HasMatchingGameplayTag(CooldownConfig.CooldownTag);
}

float UInventoryFragment_ConsumableItem::GetRemainingCooldown(UAbilitySystemComponent* ASC) const
{
	if (!ASC || !CooldownConfig.CooldownTag.IsValid())
	{
		return 0.f;
	}

	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(CooldownConfig.CooldownTag);

	FGameplayEffectQuery Query =
		FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(TagContainer);

	TArray<float> Times = ASC->GetActiveEffectsTimeRemaining(Query);

	float MaxTime = 0.f;
	for (float T : Times)
	{
		if (T > MaxTime)
		{
			MaxTime = T;
		}
	}

	return MaxTime;
}

bool UInventoryFragment_ConsumableItem::IsInUse(UAbilitySystemComponent* ASC) const
{
	if (!ASC || InUseTags.Num() == 0)
	{
		return false;
	}

	// 🔹 InUseTags 와 동일한 태그를 MainEffect GE 의 GrantedTags 에도 넣어두면,
	//     버프가 살아있는 동안 ASC 에 자동으로 붙어 있게 됨.
	return ASC->HasAnyMatchingGameplayTags(InUseTags);
}
