#include "KRCurrencyComponent.h"

#include "Data/CurrencyDataStruct.h"
#include "Data/GameDataType.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "Player/KRPlayerState.h"
#include "GameplayTag/KRShopTag.h"
#include "GameplayTag/KRStateTag.h"

#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Subsystem/KRDataTablesSubsystem.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"

DEFINE_LOG_CATEGORY(LogKRCurrency);

UKRCurrencyComponent::UKRCurrencyComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UKRCurrencyComponent::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeDataTables();
	InitializeCurrencyLossRules();
	
	BroadcastCurrencyUI();
}

void UKRCurrencyComponent::InitializeDataTables()
{
	if (DataTables) return;
	
	UGameInstance* GI = nullptr;

	if (UWorld* World = GetWorld())
	{
		GI = World->GetGameInstance();
	}

	if (!GI) return;

	DataTables = GI->GetSubsystem<UKRDataTablesSubsystem>();
}

void UKRCurrencyComponent::InitializeCurrencyLossRules()
{
	if (bCurrencyTableInitialized) return;

	bCurrencyTableInitialized = true;
	
	if (!DataTables)
	{
		if (UWorld* World = GetWorld())
		{
			if (UGameInstance* GI = World->GetGameInstance())
			{
				DataTables = GI->GetSubsystem<UKRDataTablesSubsystem>();
			}
		}
	}

	if (!DataTables) return;
	
	UCacheDataTable* Cache = DataTables->GetTable(EGameDataType::CurrencyData);
	if (!Cache) return;
	
	UDataTable* DT = Cache->GetTable();
	if (!DT) return;
	
	const FString Context(TEXT("CurrencyData_Init"));
	TArray<FCurrencyDataStruct*> Rows;
	DT->GetAllRows(Context, Rows);

	CurrencyLossRuleMap.Empty();

	for (const FCurrencyDataStruct* Row : Rows)
	{
		if (!Row) continue;
		if (!Row->CurrencyTag.IsValid()) continue;
		
		CurrencyLossRuleMap.Add(Row->CurrencyTag, Row->bLossRule);
	}
}

bool UKRCurrencyComponent::IsLostOnDeath(const FGameplayTag& CurrencyTag) const
{
	if (!CurrencyTag.IsValid()) return false;
	
	if (const bool* bLossPtr = CurrencyLossRuleMap.Find(CurrencyTag))
	{
		return *bLossPtr;
	}
	
	return false;
}

int32 UKRCurrencyComponent::GetCurrency(const FGameplayTag& CurrencyTag) const
{
	if (!CurrencyTag.IsValid())
	{
		return 0;
	}

	if (CurrencyTag.MatchesTagExact(KRTAG_CURRENCY_PURCHASE_GEARING))
	{
		return CurrentGearing;
	}
	
	if (CurrencyTag.MatchesTagExact(KRTAG_CURRENCY_SKILL_CORBYTE))
	{
		return CurrentCorbyte;
	}

	return 0;
}

int32 UKRCurrencyComponent::GetLostCurrency(const FGameplayTag& CurrencyTag) const
{
	if (!CurrencyTag.IsValid())
	{
		return 0;
	}

	if (CurrencyTag.MatchesTagExact(KRTAG_CURRENCY_PURCHASE_GEARING))
	{
		return -FMath::Abs(DeltaGearing);
	}

	return 0;
}

bool UKRCurrencyComponent::CanAfford(const FGameplayTag& CurrencyTag, int32 Amount) const
{
	if (Amount <= 0)
	{
		return true;
	}
	
	return GetCurrency(CurrencyTag) >= Amount;
}

void UKRCurrencyComponent::AddCurrency(const FGameplayTag& CurrencyTag, int32 Delta)
{
	if (Delta == 0 || !CurrencyTag.IsValid())
	{
		return;
	}

	const int32 OldValue = GetCurrency(CurrencyTag);
	const int32 NewValue = FMath::Max(0, OldValue + Delta);

	ApplyCurrencyChange_Internal(CurrencyTag, NewValue);
}

bool UKRCurrencyComponent::SpendCurrency(const FGameplayTag& CurrencyTag, int32 Cost)
{
	if (Cost <= 0 || !CurrencyTag.IsValid())
	{
		return false;
	}

	const int32 OldValue = GetCurrency(CurrencyTag);
	if (OldValue < Cost)
	{
		return false;
	}

	const int32 NewValue = OldValue - Cost;
	ApplyCurrencyChange_Internal(CurrencyTag, NewValue);
	
	return true;
}

void UKRCurrencyComponent::SetInsuranceKeepRate(float NewRate)
{
	InsuranceKeepRate = FMath::Clamp(NewRate, 0.0f, 1.0f);
}

void UKRCurrencyComponent::ClearInsurance()
{
	InsuranceKeepRate = 0.0f;
}

float UKRCurrencyComponent::GetInsuranceKeepRate() const
{
	return InsuranceKeepRate;
}

void UKRCurrencyComponent::HandleDeath()
{
	const FGameplayTag GearingTag = KRTAG_CURRENCY_PURCHASE_GEARING;
	
	if (!IsLostOnDeath(GearingTag))
	{
		return;
	}
	
	const int32 Current = FMath::Max(0, GetCurrency(GearingTag));
	
	const float KeepRate  = FMath::Clamp(GetInsuranceKeepRate(), 0.0f, 1.0f);
	const int32 KeepAmount = FMath::FloorToInt(Current * KeepRate);
	const int32 LostAmount = Current - KeepAmount;
	
	CurrentGearing = KeepAmount;
	DeltaGearing    = LostAmount;

	ApplyCurrencyChange_Internal(GearingTag, KeepAmount);
	ConsumeInsuranceEffects();
}

void UKRCurrencyComponent::ApplyCurrencyChange_Internal(const FGameplayTag& CurrencyTag, int32 NewValue)
{
	if (!CurrencyTag.IsValid())
	{
		return;
	}

	if (CurrencyTag.MatchesTagExact(KRTAG_CURRENCY_PURCHASE_GEARING))
	{
		CurrentGearing = FMath::Max(0, NewValue);
	}
	else if (CurrencyTag.MatchesTagExact(KRTAG_CURRENCY_SKILL_CORBYTE))
	{
		CurrentCorbyte = FMath::Max(0, NewValue);
	}
	else
	{
		return;
	}

	BroadcastCurrencyUI();
}

void UKRCurrencyComponent::ConsumeInsuranceEffects()
{
	ClearInsurance();

	if (!ASC) return;
	
	const FGameplayTag InsuranceActiveTag = KRTAG_STATUS_ITEM_INSURANCE_CURRENCY;

	FGameplayTagContainer TagsToRemove;
	TagsToRemove.AddTag(InsuranceActiveTag);

	ASC->RemoveActiveEffectsWithGrantedTags(TagsToRemove);
}

void UKRCurrencyComponent::BroadcastCurrencyUI() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	
	static const FGameplayTag Tag_Gearing  = KRTAG_CURRENCY_PURCHASE_GEARING;
	static const FGameplayTag Tag_Corbyte = KRTAG_CURRENCY_SKILL_CORBYTE;

	FKRUIMessage_Currency Payload;
	Payload.CurrentGearing = GetCurrency(Tag_Gearing);
	Payload.CurrentCorbyte = GetCurrency(Tag_Corbyte);
	Payload.DeltaGearing   = GetLostCurrency(Tag_Gearing);

	UGameplayMessageSubsystem::Get(World).BroadcastMessage(FKRUIMessageTags::Currency(), Payload);
}

void UKRCurrencyComponent::ForceBroadcastCurrencyUI()
{
	BroadcastCurrencyUI();
}
