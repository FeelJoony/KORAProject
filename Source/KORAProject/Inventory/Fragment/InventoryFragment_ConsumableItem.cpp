#include "Inventory/Fragment/InventoryFragment_ConsumableItem.h"

#include "GAS/AttributeSets/KRPlayerAttributeSet.h"
#include "GAS/KRAbilitySystemComponent.h"

#include "Inventory/KRInventoryItemInstance.h"
#include "Subsystem/KRDataTablesSubsystem.h"
#include "Data/ItemDataStruct.h"
#include "Data/ConsumeDataStruct.h"
#include "UObject/ConstructorHelpers.h" 

#include "GameplayTag/KRSetByCallerTag.h"
#include "Player/KRPlayerState.h"

class AKRPlayerState;

UInventoryFragment_ConsumableItem::UInventoryFragment_ConsumableItem()
{
	static ConstructorHelpers::FClassFinder<UGameplayEffect> CooldownGEClass(
		TEXT("/Game/GameplayEffects/GE_Cooldown_Base.GE_Cooldown_Base_C")
	);

	if (CooldownGEClass.Succeeded())
	{
		DefaultCooldownEffectClass = CooldownGEClass.Class;

		UE_LOG(LogTemp, Warning,
			TEXT("ConsumableFragment: Cooldown GE set to %s"),
			*GetNameSafe(DefaultCooldownEffectClass));
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("UInventoryFragment_ConsumableItem: Failed to find Cooldown GE class!"));
	}
}

void UInventoryFragment_ConsumableItem::OnInstanceCreated(UKRInventoryItemInstance* Instance)
{
	Super::OnInstanceCreated(Instance);

	ConsumeID      = -1;
	EffectConfig   = FConsumableEffectConfig();
	CooldownConfig = FConsumableCooldownConfig();
	InUseTags.Reset();

	if (!Instance) { return; }

	LoadFromDataTable(Instance);
}

bool UInventoryFragment_ConsumableItem::UseConsumable(UAbilitySystemComponent* ASC)
{
	if (!ASC) { return false; }
	
	if (IsOnCooldown(ASC)) { return false; }

	if (!CanApplyMoreStacks(ASC)) { return false; }
	
	float EffectDuration = 0.f;
	if (!ApplyMainEffect(ASC, EffectDuration)) { return false; }
	
	ApplyCooldown(ASC);

	return true;
}

bool UInventoryFragment_ConsumableItem::IsOnCooldown(UAbilitySystemComponent* ASC) const
{
	if (!ASC || !CooldownConfig.CooldownTag.IsValid()) { return false; }

	return ASC->HasMatchingGameplayTag(CooldownConfig.CooldownTag);
}

float UInventoryFragment_ConsumableItem::GetRemainingCooldown(UAbilitySystemComponent* ASC) const
{
	if (!ASC || !CooldownConfig.CooldownTag.IsValid()) { return 0.f; }

	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(CooldownConfig.CooldownTag);

	FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(TagContainer);

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
	if (!ASC || InUseTags.Num() == 0) { return false; }
	
	return ASC->HasAnyMatchingGameplayTags(InUseTags);
}

void UInventoryFragment_ConsumableItem::LoadFromDataTable(UKRInventoryItemInstance* Instance)
{
	UObject* ContextObj = Instance->GetOwnerContext();
	if (!ContextObj) { return; }

	UGameInstance* GI = Cast<UGameInstance>(ContextObj);
	if (!GI) { return; }

	UKRDataTablesSubsystem* DT = GI->GetSubsystem<UKRDataTablesSubsystem>();
	if (!DT) { return; }
	
	const FGameplayTag ItemTag = Instance->GetItemTag();
	FItemDataStruct* ItemRow = DT->GetData<FItemDataStruct>(EGameDataType::ItemData, ItemTag);
	if (!ItemRow) { return; }

	ConsumeID = ItemRow->ConsumeID;
	if (ConsumeID < 0) { return; }
	
	FConsumeDataStruct* ConsumeRow = DT->GetData<FConsumeDataStruct>(EGameDataType::ConsumeData, ConsumeID);

	if (!ConsumeRow) { return; }

	EffectConfig.MainEffectClass = ConsumeRow->MainEffectClass.LoadSynchronous();
	EffectConfig.EffectType      = ConsumeRow->EffectType;
	EffectConfig.Power           = ConsumeRow->Power;
	EffectConfig.Duration        = ConsumeRow->Duration;
	EffectConfig.StackMax       = FMath::Max(ConsumeRow->StackMax, 1);

	CooldownConfig.CooldownEffectClass = DefaultCooldownEffectClass;
	
	CooldownConfig.ExtraCooldown            = ConsumeRow->CooldownDuration;
	CooldownConfig.CooldownTag              = ConsumeRow->CooldownTag;

	InUseTags = ConsumeRow->InUseTags;
}

bool UInventoryFragment_ConsumableItem::ApplyMainEffect(UAbilitySystemComponent* ASC, float& OutDuration)
{
	OutDuration = 0.f;

	if (!ASC || !EffectConfig.MainEffectClass) { return false; }

	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(EffectConfig.MainEffectClass, 1.0f, Context);

	if (!SpecHandle.IsValid()) { return false; }

	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
	if (!Spec) { return false; }
	
	if (EffectConfig.Power != 0.f)
	{
		Spec->SetSetByCallerMagnitude(KRTAG_SETBYCALLER_CONSUME_POWER, EffectConfig.Power);
	}

	if (EffectConfig.Duration > 0.f)
	{
		Spec->SetSetByCallerMagnitude(KRTAG_SETBYCALLER_CONSUME_DURATION, EffectConfig.Duration);
	}
	
	if (EffectConfig.EffectType != EConsumableEffectType::Instant && InUseTags.Num() > 0)
	{
		Spec->DynamicGrantedTags.AppendTags(InUseTags);
	}

	const FGameplayTag InsuranceActiveTag =
			FGameplayTag::RequestGameplayTag(TEXT("Status.Item.Insurance.Currency"));

	const bool bIsInsuranceConsumable =
		(EffectConfig.EffectType == EConsumableEffectType::Infinite) &&
		InUseTags.HasTag(InsuranceActiveTag);

	if (bIsInsuranceConsumable)
	{
		if (AActor* OwnerActor = ASC->GetOwnerActor())
		{
			if (AKRPlayerState* KRPS = Cast<AKRPlayerState>(OwnerActor))
			{
				if (UKRCurrencyComponent* CurComp = KRPS->GetCurrencyComponentSet())
				{
					CurComp->SetInsuranceKeepRate(EffectConfig.Power);
				}
			}
		}
	}
	
	if (EffectConfig.EffectType == EConsumableEffectType::Instant)
	{
		ASC->ApplyGameplayEffectSpecToSelf(*Spec);
		OutDuration = 0.f;
		return true;
	}
	else
	{
		FActiveGameplayEffectHandle Handle = ASC->ApplyGameplayEffectSpecToSelf(*Spec);
		OutDuration = EffectConfig.Duration;
		return Handle.IsValid();
	}
}

bool UInventoryFragment_ConsumableItem::ApplyCooldown(UAbilitySystemComponent* ASC) const
{
	if (!ASC || !CooldownConfig.CooldownEffectClass) { return false; }
	
	const float FinalCooldown = CooldownConfig.ExtraCooldown;

	if (FinalCooldown <= 0.f) { return false; }

	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle =
		ASC->MakeOutgoingSpec(CooldownConfig.CooldownEffectClass, 1.0f, Context);

	if (!SpecHandle.IsValid()) { return false; }

	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
	if (!Spec) { return false; }
	
	Spec->SetSetByCallerMagnitude(KRTAG_SETBYCALLER_CONSUME_COOLDOWN, FinalCooldown);
	
	if (CooldownConfig.CooldownTag.IsValid())
	{
		Spec->DynamicGrantedTags.AddTag(CooldownConfig.CooldownTag);
	}

	const FActiveGameplayEffectHandle Handle =
		ASC->ApplyGameplayEffectSpecToSelf(*Spec);

	return Handle.IsValid();
}

int32 UInventoryFragment_ConsumableItem::GetCurrentStacks(UAbilitySystemComponent* ASC) const
{
	if (!ASC || InUseTags.Num() == 0)
	{
		return 0;
	}
	
	FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(InUseTags);

	TArray<FActiveGameplayEffectHandle> Handles = ASC->GetActiveEffects(Query);

	int32 TotalStacks = 0;

	for (const FActiveGameplayEffectHandle& Handle : Handles)
	{
		if (!Handle.IsValid())
		{
			continue;
		}
		
		const FActiveGameplayEffect* ActiveGE = ASC->GetActiveGameplayEffect(Handle);
		if (!ActiveGE)
		{
			continue;
		}

		const int32 GEStack = ActiveGE->Spec.StackCount;

		TotalStacks += FMath::Max(GEStack, 1);
	}

	return TotalStacks;
}


bool UInventoryFragment_ConsumableItem::CanApplyMoreStacks(UAbilitySystemComponent* ASC) const
{
	if (!ASC) { return false; }
	
	if (EffectConfig.EffectType == EConsumableEffectType::Instant) { return true; }
	
	if (EffectConfig.StackMax <= 0) { return true; }

	const int32 CurrentStacks = GetCurrentStacks(ASC);
	const int32 MaxStacks     = FMath::Max(EffectConfig.StackMax, 1);

	return CurrentStacks < MaxStacks;
}