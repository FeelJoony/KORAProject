#include "PlayAbilityChecker.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Subsystem/KRQuestSubsystem.h"
#include "Quest/KRQuestInstance.h"

UPlayAbilityChecker::UPlayAbilityChecker()
{
}

void UPlayAbilityChecker::Initialize(FGameplayTag InTag)
{
	APlayerState* PlayerState = UGameplayStatics::GetPlayerState(GetWorld(), 0);

	if (PlayerState)
	{
		IAbilitySystemInterface* Interface = CastChecked<IAbilitySystemInterface>(PlayerState);
		UAbilitySystemComponent* ASC = Interface->GetAbilitySystemComponent();
		if (ASC != nullptr)
		{
			ASC->RegisterGameplayTagEvent(InTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::Count);
		}
	}
}

bool UPlayAbilityChecker::CanCount(const FSubQuestEvalDataStruct& EvalData, const FGameplayTag& InTag)
{
	APlayerState* PlayerState = UGameplayStatics::GetPlayerState(GetWorld(), 0);

	if (PlayerState)
	{
		IAbilitySystemInterface* Interface = CastChecked<IAbilitySystemInterface>(PlayerState);
		UAbilitySystemComponent* ASC = Interface->GetAbilitySystemComponent();

		if (ASC != nullptr && ASC->HasMatchingGameplayTag(InTag))
		{
			return true;
		}
	}
	
	return false;
}

void UPlayAbilityChecker::Count(const FGameplayTag InTag, int32 NewCount)
{
	if (InTag.IsValid() == false)
	{
		return;
	}
	
	if (NewCount > 0)
	{
		UKRQuestSubsystem& QuestSubsystem = UKRQuestSubsystem::Get(this);
		UKRQuestInstance* QuestInstance = QuestSubsystem.GetActiveInstance();
		if (QuestInstance)
		{
			QuestInstance->AddCount();
		}
	}
}
