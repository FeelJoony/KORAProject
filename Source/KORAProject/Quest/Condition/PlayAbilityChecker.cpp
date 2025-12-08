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
	PS = UGameplayStatics::GetPlayerState(GetWorld(), 0);

	if (PS)
	{
		IAbilitySystemInterface* Interface = CastChecked<IAbilitySystemInterface>(PS);
		ASC = Interface->GetAbilitySystemComponent();
		if (ASC)
		{
			OnTagCountChangedDelegateHandle = ASC->RegisterGameplayTagEvent(ObjectiveTag).AddLambda([&](const FGameplayTag Tag, int32 NewCount)
			{
				if (ObjectiveTag.IsValid() == false)
				{
					return;
				}

				if (!CanCount(EvalData, Tag))
				{
					return;
				}
	
				if (NewCount > 0)
				{
					if (QuestInstance)
					{
						if (QuestInstance->GetSubQuestEvalDataStruct().ObjectiveTag != ObjectiveTag)
						{
							return;
						}
		
						QuestInstance->AddCount();
					}
				}
			});
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
		if (ASC != nullptr && ASC->HasMatchingGameplayTag(InTag))
		{
			return true;
		}
	}
	
	return false;
}
