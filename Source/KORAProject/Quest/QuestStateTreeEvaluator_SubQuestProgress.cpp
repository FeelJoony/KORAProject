#include "QuestStateTreeEvaluator_SubQuestProgress.h"
#include "KRQuestActor.h"
#include "KRQuestInstance.h"
#include "StateTreeExecutionContext.h"

DEFINE_LOG_CATEGORY(LogQuestStateTreeEvaluator_SubQuestProgress);

void UQuestStateTreeEvaluator_SubQuestProgress::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);
	
	UE_LOG(LogQuestStateTreeEvaluator_SubQuestProgress, Warning, TEXT("========================================"));
	UE_LOG(LogQuestStateTreeEvaluator_SubQuestProgress, Warning, TEXT("[QuestEvaluator] TreeStart called!"));
	UE_LOG(LogQuestStateTreeEvaluator_SubQuestProgress, Warning, TEXT("========================================"));

	// Context Owner 확인
	UObject* ContextOwner = Context.GetOwner();
	UE_LOG(LogQuestStateTreeEvaluator_SubQuestProgress, Log, TEXT("[QuestEvaluator] Context Owner: %s (Class: %s)"),
		ContextOwner ? *ContextOwner->GetName() : TEXT("NULL"),
		ContextOwner ? *ContextOwner->GetClass()->GetName() : TEXT("NULL"));

	AKRQuestActor* QuestActor = CastChecked<AKRQuestActor>(Actor);
	UKRQuestInstance* QuestInstance = QuestActor->GetQuestInstance();
	if (QuestInstance)
	{
		QuestInstance->StartQuest();
	}
	
	Reset();	
}

void UQuestStateTreeEvaluator_SubQuestProgress::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);

	AKRQuestActor* QuestActor = CastChecked<AKRQuestActor>(Actor);
	UKRQuestInstance* QuestInstance = QuestActor->GetQuestInstance();
	if (QuestInstance)
	{
		CurrentOrder = QuestInstance->GetSubQuestEvalData().EvalData.OrderIndex;
		
		QuestInstance->TickQuest(DeltaTime);
	}
}

void UQuestStateTreeEvaluator_SubQuestProgress::TreeStop(FStateTreeExecutionContext& Context)
{
	Reset();

	Super::TreeStop(Context);
}

void UQuestStateTreeEvaluator_SubQuestProgress::Reset()
{
	CurrentOrder = 0;
}
