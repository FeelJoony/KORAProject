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

UQuestConditionChecker* UPlayAbilityChecker::Initialize(UKRQuestInstance* NewQuestInstance, const FSubQuestEvalDataStruct& EvalData)
{
	ObjectiveTag = EvalData.ObjectiveTag;
	QuestInstance = NewQuestInstance;
	PS = UGameplayStatics::GetPlayerState(NewQuestInstance, 0);

	if (PS)
	{
		IAbilitySystemInterface* Interface = CastChecked<IAbilitySystemInterface>(PS);
		ASC = Interface->GetAbilitySystemComponent();
		if (ASC)
		{
			OnTagCountChangedDelegateHandle = ASC->RegisterGameplayTagEvent(ObjectiveTag).AddUObject(this, &ThisClass::ReceiveMessage);
		}
	}

	return this;
}

void UPlayAbilityChecker::Uninitialize()
{
	if (ASC)
	{
		if (OnTagCountChangedDelegateHandle.IsValid())
		{
			ASC->UnregisterGameplayTagEvent(OnTagCountChangedDelegateHandle, ObjectiveTag);
		}
		
	}
}

bool UPlayAbilityChecker::CanCount(const FSubQuestEvalDataStruct& EvalData, const FGameplayTag& InTag)
{
	if (ASC)
	{
		return ASC->HasMatchingGameplayTag(EvalData.ObjectiveTag);
	}
	
	return false;
}

void UPlayAbilityChecker::ReceiveMessage(const FGameplayTag InTag, int32 Count)
{
	if (ObjectiveTag.IsValid() == false)
	{
		return;
	}

	const FSubQuestEvalDataStruct& EvalData = QuestInstance->GetSubQuestEvalDataStruct();
	if (!CanCount(EvalData, InTag))
	{
		return;
	}
	
	QuestInstance->AddCount();
}
