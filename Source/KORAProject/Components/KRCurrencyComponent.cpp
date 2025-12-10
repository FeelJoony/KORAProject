#include "KRCurrencyComponent.h"

#include "Data/GameDataType.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "GAS/AttributeSets/KRPlayerAttributeSet.h"
#include "Player/KRPlayerState.h"

#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "SubSystem/KRDataTablesSubsystem.h"

DEFINE_LOG_CATEGORY(LogKRCurrency);

UKRCurrencyComponent::UKRCurrencyComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UKRCurrencyComponent::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeAbilityReferences();
	InitializeDataTables();
}

void UKRCurrencyComponent::InitializeAbilityReferences()
{
	if (ASC && AttributeSet) return;
	
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor) return;
	
	if (AKRPlayerState* KRPS = Cast<AKRPlayerState>(OwnerActor))
	{
		if (UAbilitySystemComponent* ASCBase = KRPS->GetAbilitySystemComponent())
		{
			if (UKRAbilitySystemComponent* KRASC = Cast<UKRAbilitySystemComponent>(ASCBase))
			{
				ASC = KRASC;
				AttributeSet = ASC->GetSet<UKRPlayerAttributeSet>();
			}
		}
	}
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

bool UKRCurrencyComponent::GetAttributeForCurrencyTag(
	const FGameplayTag& CurrencyTag,
	FGameplayAttribute& OutAttribute) const
{
	if (!ASC) return false;

	static const FGameplayTag Tag_Gearing =
		FGameplayTag::RequestGameplayTag(TEXT("Currency.Purchase.Gearing"));
	static const FGameplayTag Tag_Corbyte =
		FGameplayTag::RequestGameplayTag(TEXT("Currency.Skill.Corbyte"));

	if (CurrencyTag.MatchesTagExact(Tag_Gearing))
	{
		OutAttribute = UKRPlayerAttributeSet::GetGearingCurrentAttribute();
		return true;
	}

	if (CurrencyTag.MatchesTagExact(Tag_Corbyte))
	{
		OutAttribute = UKRPlayerAttributeSet::GetCorbyteCurrentAttribute();
		return true;
	}

	return false;
}

bool UKRCurrencyComponent::GetLostAttributeForCurrencyTag(
	const FGameplayTag& CurrencyTag,
	FGameplayAttribute& OutAttribute) const
{
	if (!ASC) return false;

	static const FGameplayTag Tag_Gearing =
		FGameplayTag::RequestGameplayTag(TEXT("Currency.Purchase.Gearing"));

	if (CurrencyTag.MatchesTagExact(Tag_Gearing))
	{
		OutAttribute = UKRPlayerAttributeSet::GetGearingLostAttribute();
		return true;
	}

	return false;
}

bool UKRCurrencyComponent::IsLostOnDeath(const FGameplayTag& CurrencyTag) const
{
	const_cast<UKRCurrencyComponent*>(this)->InitializeCurrencyLossRules();

	if (!CurrencyTag.IsValid()) return false;
	
	if (const bool* bLossPtr = CurrencyLossRuleMap.Find(CurrencyTag))
	{
		return *bLossPtr;
	}
	
	return false;
}

int32 UKRCurrencyComponent::GetCurrency(const FGameplayTag& CurrencyTag) const
{
	if (!ASC || !AttributeSet || !CurrencyTag.IsValid()) return 0;

	FGameplayAttribute Attr;
	if (!GetAttributeForCurrencyTag(CurrencyTag, Attr)) return 0;

	const float Value = ASC->GetNumericAttribute(Attr);
	return static_cast<int32>(Value);
}

int32 UKRCurrencyComponent::GetLostCurrency(const FGameplayTag& CurrencyTag) const
{
	if (!ASC || !CurrencyTag.IsValid()) return 0;
	
	FGameplayAttribute LostAttr;
	if (!GetLostAttributeForCurrencyTag(CurrencyTag, LostAttr)) return 0;
	
	const float Value = ASC->GetNumericAttribute(LostAttr);
	return static_cast<int32>(Value);
}

bool UKRCurrencyComponent::CanAfford(const FGameplayTag& CurrencyTag, int32 Amount) const
{
	if (Amount <= 0) return true;
	
	return GetCurrency(CurrencyTag) >= Amount;
}

void UKRCurrencyComponent::AddCurrency(const FGameplayTag& CurrencyTag, int32 Delta)
{
	if (Delta == 0 || !CurrencyTag.IsValid()) return;

	const int32 OldValue = GetCurrency(CurrencyTag);
	const int32 NewValue = FMath::Max(0, OldValue + Delta);

	ApplyCurrencyChange(CurrencyTag, NewValue);
}

bool UKRCurrencyComponent::SpendCurrency(const FGameplayTag& CurrencyTag, int32 Cost)
{
	if (Cost <= 0 || !CurrencyTag.IsValid()) return false;

	const int32 OldValue = GetCurrency(CurrencyTag);
	if (OldValue < Cost) return false;

	const int32 NewValue = OldValue - Cost;
	ApplyCurrencyChange(CurrencyTag, NewValue);
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
	if (!ASC) return;

	const FGameplayTag GearingTag = FGameplayTag::RequestGameplayTag(TEXT("Currency.Purchase.Gearing"));
	
	if (!IsLostOnDeath(GearingTag)) return;
	
	const int32 CurrentGearing = FMath::Max(0, GetCurrency(GearingTag));
	
	const float KeepRate = FMath::Clamp(GetInsuranceKeepRate(), 0.0f, 1.0f);
	const float LossRate = 1.0f - KeepRate;
	
	const int32 KeepAmount = FMath::FloorToInt(CurrentGearing * KeepRate);
	const int32 LostAmount = CurrentGearing - KeepAmount;
	
	ApplyCurrencyChange(GearingTag, KeepAmount);
	
	FGameplayAttribute LostAttr;
	if (GetLostAttributeForCurrencyTag(GearingTag, LostAttr))
	{
		const float NewLost = static_cast<float>(LostAmount);
		ASC->SetNumericAttributeBase(LostAttr, NewLost);
	}
	
	ConsumeInsuranceEffects();
}

void UKRCurrencyComponent::ApplyCurrencyChange(
	const FGameplayTag& CurrencyTag, int32 NewValue)
{
	if (!ASC || !AttributeSet || !CurrencyTag.IsValid()) return;

	FGameplayAttribute Attr;
	if (!GetAttributeForCurrencyTag(CurrencyTag, Attr)) return;
	
	const int32 OldValue = GetCurrency(CurrencyTag);
	ASC->SetNumericAttributeBase(Attr, static_cast<float>(NewValue));

	// TODO: 나중에 여기서 UI Message Broadcast 하면 될거 같음.
}

void UKRCurrencyComponent::ConsumeInsuranceEffects()
{
	ClearInsurance();

	if (!ASC) return;

	const FGameplayTag InsuranceActiveTag = FGameplayTag::RequestGameplayTag(TEXT("Status.Item.Insurance.Currency"));

	FGameplayTagContainer TagsToRemove;
	TagsToRemove.AddTag(InsuranceActiveTag);

	ASC->RemoveActiveEffectsWithGrantedTags(TagsToRemove);
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

