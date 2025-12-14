#include "Quest/Condition/KillMonsterChecker.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayTag/KREnemyTag.h"
#include "GAS/Abilities/EnemyAbility/KRGA_Enemy_Die.h"
#include "Quest/KRQuestInstance.h"

UKillMonsterChecker::UKillMonsterChecker()
{

}

UQuestConditionChecker* UKillMonsterChecker::Initialize(UKRQuestInstance* NewQuestInstance, const FSubQuestEvalDataStruct& EvalData)
{
	ObjectiveTag = EvalData.ObjectiveTag;
	QuestInstance = NewQuestInstance;

	UGameplayMessageSubsystem& Subsystem = UGameplayMessageSubsystem::Get(QuestInstance);
	Subsystem.RegisterListener(ObjectiveTag, this, &UKillMonsterChecker::ReceiveMessage);

	return this;
}

void UKillMonsterChecker::Uninitialize()
{
	
}

bool UKillMonsterChecker::CanCount(const FSubQuestEvalDataStruct& EvalData, const FGameplayTag& InTag)
{
	return true;
}

void UKillMonsterChecker::ReceiveMessage(FGameplayTag InTag, const FKillMonsterMessage& Message)
{
	QuestInstance->AddCount();
}