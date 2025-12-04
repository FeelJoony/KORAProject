#include "Inventory/Fragment/InventoryFragment_ConsumableItem.h"

#include "Inventory/KRInventoryItemInstance.h"
#include "Subsystem/KRDataTablesSubsystem.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayTag/KRSetByCallerTag.h"
#include "Data/ConsumeDataStruct.h" // 네가 만든 FConsumeItemDataStruct 헤더
#include "Data/ItemDataStruct.h"
#include "SubSystem/KRInventorySubsystem.h"

struct FItemDataStruct;

void UInventoryFragment_ConsumableItem::OnInstanceCreated(UKRInventoryItemInstance* Instance)
{
	Super::OnInstanceCreated(Instance);

	if (!Instance)
	{
		UE_LOG(LogInventorySubSystem, Warning,
			TEXT("ConsumableItem::OnInstanceCreated: Instance is null"));
		return;
	}

	// 1) 아직 ConsumeID가 설정되지 않았다면 → ItemData에서 찾아서 채워준다.
	if (ConsumeID < 0)
	{
		if (UWorld* World = Instance->GetWorld())
		{
			if (UGameInstance* GI = World->GetGameInstance())
			{
				if (UKRDataTablesSubsystem* DataTables = GI->GetSubsystem<UKRDataTablesSubsystem>())
				{
					const FGameplayTag ItemTag = Instance->GetItemTag();

					// 🔥 ItemData에서 이 아이템의 ConsumeID를 읽어오기
					if (FItemDataStruct* ItemRow =
						DataTables->GetData<FItemDataStruct>(EGameDataType::ItemData, ItemTag))
					{
						ConsumeID = ItemRow->ConsumeID;
						UE_LOG(LogInventorySubSystem, Log,
							TEXT("ConsumableItem::OnInstanceCreated: ItemTag=%s -> ConsumeID=%d"),
							*ItemTag.ToString(), ConsumeID);
					}
					else
					{
						UE_LOG(LogInventorySubSystem, Warning,
							TEXT("ConsumableItem::OnInstanceCreated: No ItemData for Tag=%s"),
							*ItemTag.ToString());
					}
				}
			}
		}
	}

	// 2) 그래도 여전히 유효하지 않으면 포기
	if (ConsumeID < 0)
	{
		UE_LOG(LogInventorySubSystem, Warning,
			TEXT("ConsumableItem::OnInstanceCreated: Invalid ConsumeID (ItemTag=%s)"),
			*Instance->GetItemTag().ToString());
		return;
	}

	// 3) 이제 ConsumeData 테이블에서 GE/쿨다운/태그 설정 로드
	LoadFromDataTable(Instance);
}


void UInventoryFragment_ConsumableItem::LoadFromDataTable(UKRInventoryItemInstance* Instance)
{
	if (!Instance)
	{
		return;
	}

	UWorld* World = Instance->GetWorld();
	if (!World) { return; }

	UGameInstance* GI = World->GetGameInstance();
	if (!GI) { return; }

	UKRDataTablesSubsystem* DataTables = GI->GetSubsystem<UKRDataTablesSubsystem>();
	if (!DataTables) { return; }

	// ✅ 여기 enum 값만 네 프로젝트에 맞게 바꿔주면 됨
	FConsumeDataStruct* Row =
		DataTables->GetData<FConsumeDataStruct>(EGameDataType::ConsumeData, ConsumeID);

	if (!Row)
	{
		UE_LOG(LogTemp, Warning, TEXT("ConsumableItem: No ConsumeData for ID=%d"), ConsumeID);
		return;
	}

	// EffectConfig 채우기
	EffectConfig.MainEffectClass = Row->MainEffectClass.LoadSynchronous();
	EffectConfig.EffectType      = Row->EffectType;
	EffectConfig.Power           = Row->Power;
	EffectConfig.Duration        = Row->Duration;

	// Cooldown 설정
	CooldownConfig.CooldownEffectClass      = Row->CooldownEffectClass.LoadSynchronous();
	CooldownConfig.ExtraCooldown            = Row->CooldownDuration;
	CooldownConfig.CooldownTag              = Row->CooldownTag;
	CooldownConfig.bIncludeDurationInCooldown = Row->bIncludeDurationInCooldown;

	// InUseTags
	InUseTags = Row->InUseTags;
}

bool UInventoryFragment_ConsumableItem::UseConsumable(UAbilitySystemComponent* ASC)
{
	if (!ASC)
	{
		UE_LOG(LogInventorySubSystem, Warning, TEXT("UseConsumable: ASC is null"));
		return false;
	}

	if (IsOnCooldown(ASC))
	{
		const float Remaining = GetRemainingCooldown(ASC);
		UE_LOG(LogInventorySubSystem, Warning,
			TEXT("UseConsumable: On Cooldown (%.1f sec left)"), Remaining);
		return false;
	}

	if (IsInUse(ASC))
	{
		UE_LOG(LogInventorySubSystem, Warning,
			TEXT("UseConsumable: Already in use (InUseTags active)"));
		return false;
	}

	if (RequiredTags.Num() > 0 && !ASC->HasAllMatchingGameplayTags(RequiredTags))
	{
		UE_LOG(LogInventorySubSystem, Warning,
			TEXT("UseConsumable: RequiredTags not met. TagCount=%d"),
			RequiredTags.Num());
		return false;
	}

	float EffectDuration = 0.f;
	const bool bMainEffectOK = ApplyMainEffect(ASC, EffectDuration);
	if (!bMainEffectOK)
	{
		UE_LOG(LogInventorySubSystem, Warning,
			TEXT("UseConsumable: ApplyMainEffect FAILED. ConsumeID=%d"), ConsumeID);
		return false;
	}

	const bool bCDOK = ApplyCooldown(ASC, EffectDuration);
	UE_LOG(LogInventorySubSystem, Log,
		TEXT("UseConsumable: Success. Duration=%.2f, CooldownApplied=%s"),
		EffectDuration,
		bCDOK ? TEXT("true") : TEXT("false"));

	return true;
}


bool UInventoryFragment_ConsumableItem::ApplyMainEffect(UAbilitySystemComponent* ASC, float& OutDuration)
{
	OutDuration = 0.f;

	if (!ASC)
	{
		UE_LOG(LogInventorySubSystem, Warning, TEXT("ApplyMainEffect: ASC is null"));
		return false;
	}

	if (!EffectConfig.MainEffectClass)
	{
		UE_LOG(LogInventorySubSystem, Warning,
			TEXT("ApplyMainEffect: MainEffectClass is NULL. ConsumeID=%d"), ConsumeID);
		return false;
	}
	
	if (!ASC || !EffectConfig.MainEffectClass)
	{
		return false;
	}

	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	Context.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
		EffectConfig.MainEffectClass,
		1.0f,
		Context
	);

	if (!SpecHandle.IsValid())
	{
		return false;
	}

	// SetByCaller: Power
	if (EffectConfig.Power != 0.f)
	{
		SpecHandle.Data->SetSetByCallerMagnitude(
			KRTAG_SETBYCALLER_CONSUME_POWER,
			EffectConfig.Power
		);
	}

	// SetByCaller: Duration
	if (EffectConfig.EffectType == EConsumableEffectType::HasDuration && EffectConfig.Duration > 0.f)
	{
		SpecHandle.Data->SetSetByCallerMagnitude(
			KRTAG_SETBYCALLER_CONSUME_DURATION,
			EffectConfig.Duration
		);
		OutDuration = EffectConfig.Duration;
	}

	FActiveGameplayEffectHandle Handle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	if (!Handle.IsValid())
	{
		return false;
	}

	return true;
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
		FinalCooldown += FMath::Max(0.f, EffectDuration);
	}

	if (FinalCooldown <= 0.f)
	{
		return false;
	}

	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	Context.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
		CooldownConfig.CooldownEffectClass,
		1.0f,
		Context
	);

	if (!SpecHandle.IsValid())
	{
		return false;
	}

	// SetByCaller: Cooldown Duration
	SpecHandle.Data->SetSetByCallerMagnitude(
		KRTAG_SETBYCALLER_COOLDOWN_DURATION,
		FinalCooldown
	);

	FActiveGameplayEffectHandle Handle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	if (!Handle.IsValid())
	{
		return false;
	}

	return true;
}

bool UInventoryFragment_ConsumableItem::IsOnCooldown(UAbilitySystemComponent* ASC) const
{
	if (!ASC || !CooldownConfig.CooldownTag.IsValid())
	{
		return false;
	}

	// Cooldown GE가 Owner에게 CooldownTag를 부여한다고 가정
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

	const FGameplayEffectQuery Query =
		FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(TagContainer);

	TArray<float> Times = ASC->GetActiveEffectsTimeRemaining(Query);
	if (Times.Num() == 0)
	{
		return 0.f;
	}

	float MaxTime = 0.f;
	for (float T : Times)
	{
		MaxTime = FMath::Max(MaxTime, T);
	}
	return MaxTime;
}

bool UInventoryFragment_ConsumableItem::IsInUse(UAbilitySystemComponent* ASC) const
{
	if (!ASC || InUseTags.Num() == 0)
	{
		return false;
	}

	return ASC->HasAnyMatchingGameplayTags(InUseTags);
}
