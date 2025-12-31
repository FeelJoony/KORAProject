#include "MainQuestDelegate.h"

#include "GameFramework/GameplayMessageSubsystem.h"
#include "Quest/KRQuestInstance.h"
#include "Data/SubQuestDataStruct.h"
#include "SubSystem/KRQuestSubsystem.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"

void UMainQuestDelegate::Initialize(class UKRQuestInstance* NewQuestInstance)
{
	Super::Initialize(NewQuestInstance);

	OnQuestAccepted.AddDynamic(this, &ThisClass::OnAppearMainQuestUI);
	OnQuestSetNextSub.AddDynamic(this, &ThisClass::OnAppearMainQuestUI);
	OnQuestCompleted.AddDynamic(this, &ThisClass::OnMainQuestCompleted);
}

void UMainQuestDelegate::OnAppearMainQuestUI(int32 OrderIndex)
{
	if (!QuestInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MainQuestDelegate] QuestInstance is null!"));
		return;
	}

	const FSubQuestDataStruct& SubQuestDataStruct = QuestInstance->GetSubQuestData();
	const FSubQuestEvalDataStruct& SubQuestEvalData = SubQuestDataStruct.GetEvalData(OrderIndex);
	
	if (SubQuestEvalData.UIRowName.IsValid())
	{
		FKRUIMessage_Quest QuestMessage;
		QuestMessage.QuestNameKey = SubQuestEvalData.UIRowName;
		UGameplayMessageSubsystem::Get(GetWorld()).BroadcastMessage(FKRUIMessageTags::Quest(), QuestMessage);
	}
}

void UMainQuestDelegate::OnMainQuestCompleted(int32 OrderIndex)
{
	if (!QuestInstance)
	{
		return;
	}

	const FQuestDataStruct& QuestData = QuestInstance->GetQuestData();
	const int32 CurrentQuestIndex = QuestData.Index;

	const FSubQuestDataStruct& SubQuestData = QuestInstance->GetSubQuestData();
	const int32 TotalSubQuests = SubQuestData.GetEvalDataCount();
	
	if (OrderIndex >= TotalSubQuests)
	{
		UKRQuestSubsystem& QuestSubsystem = UKRQuestSubsystem::Get(GetWorld());
		const int32 NextQuestIndex = CurrentQuestIndex + 1;
		constexpr int32 FinalQuestIndex = 3;

		if (CurrentQuestIndex < FinalQuestIndex)
		{
			// TODO: NPC 대화 레벨로 전환
		}
		else
		{
			// TODO: 엔딩 전환
		}
	}
}
