#include "QuestStateTreeSchema.h"
#include "KRQuestActor.h"
#include "StateTreeTaskBase.h"

TArray<FStateTreeExternalDataDesc> UQuestStateTreeSchema::ContextDataDescs;

UQuestStateTreeSchema::UQuestStateTreeSchema()
{
	ContextActorClass = AKRQuestActor::StaticClass();

	if (ContextDataDescs.IsEmpty())
	{
		ContextDataDescs.Add(FStateTreeExternalDataDesc(
			FName(TEXT("QuestActor")),
			AKRQuestActor::StaticClass(),
			FGuid::NewGuid()
			));
	}
}

TConstArrayView<FStateTreeExternalDataDesc> UQuestStateTreeSchema::GetContextDataDescs() const
{
	return ContextDataDescs;
}

bool UQuestStateTreeSchema::IsStructAllowed(const UScriptStruct* InStruct) const
{
	// if (!InStruct)
	// {
	// 	return false;
	// }

	// Quest 전용 노드만 허용 (이름이 "Quest"로 시작하는 노드)
	// 또는 기본 StateTree 노드는 허용
	// const FString StructName = InStruct->GetName();
	//
	// // Quest 관련 노드
	// if (StructName.StartsWith(TEXT("QuestStateTree")) ||
	//     StructName.StartsWith(TEXT("FQuestStateTree")))
	// {
	// 	return true;
	// }
	//
	// // 기본 StateTree 노드 (CommonBase 등)
	// if (InStruct->IsChildOf(FStateTreeTaskCommonBase::StaticStruct()) ||
	//     InStruct->IsChildOf(FStateTreeConditionCommonBase::StaticStruct()) ||
	//     InStruct->IsChildOf(FStateTreeEvaluatorCommonBase::StaticStruct()))
	// {
	// 	return true;
	// }

	return Super::IsStructAllowed(InStruct);
}

bool UQuestStateTreeSchema::IsClassAllowed(const UClass* InClass) const
{
	// if (!InClass)
	// {
	// 	return false;
	// }
	//
	// // Quest 관련 클래스만 허용
	// const FString ClassName = InClass->GetName();
	// if (ClassName.StartsWith(TEXT("Quest")) ||
	//     ClassName.StartsWith(TEXT("KRQuest")))
	// {
	// 	return true;
	// }

	return Super::IsClassAllowed(InClass);
}

bool UQuestStateTreeSchema::IsExternalItemAllowed(const UStruct& InStruct) const
{
	// Quest 관련 클래스만 허용
	const FString ClassName = InStruct.GetName();
	if (ClassName.StartsWith(TEXT("Quest")) ||
		ClassName.StartsWith(TEXT("SubQuest")) ||
		ClassName.StartsWith(TEXT("KRQuest")))
	{
		return true;
	}
	
	return Super::IsExternalItemAllowed(InStruct);
}
