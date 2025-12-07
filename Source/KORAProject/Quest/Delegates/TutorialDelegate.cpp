#include "TutorialDelegate.h"

#include "GameFramework/GameplayMessageSubsystem.h"
#include "Quest/KRQuestInstance.h"
#include "Data/SubQuestDataStruct.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"

void UTutorialDelegate::Initialize(class UKRQuestInstance* NewQuestInstance)
{
	Super::Initialize(NewQuestInstance);

	OnQuestAccepted.AddDynamic(this, &ThisClass::OnAppearTutorialUI);
	OnQuestSetNextSub.AddDynamic(this, &ThisClass::OnAppearTutorialUI);
}

void UTutorialDelegate::OnAppearTutorialUI(int32 OrderIndex)
{
	const FSubQuestDataStruct& SubQuestDataStruct = QuestInstance->GetSubQuestData();
	const FSubQuestEvalDataStruct& EvalDataStruct = SubQuestDataStruct.GetEvalData(OrderIndex);
	if (EvalDataStruct.UIRowName.IsValid())
	{
		FKRUIMessage_Tutorial TutorialMessage;
		TutorialMessage.TutorialDTRowName = EvalDataStruct.UIRowName;
		UGameplayMessageSubsystem::Get(GetWorld()).BroadcastMessage(FKRUIMessageTags::Tutorial(), TutorialMessage);	
	}
}
