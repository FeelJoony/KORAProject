// Fill out your copyright notice in the Description page of Project Settings.

#include "QuestStateTreeSchema.h"
#include "KRQuestComponent.h"
#include "KRQuestInstance.h"
#include "StateTreeTaskBase.h"
#include "StateTreeConditionBase.h"
#include "StateTreeEvaluatorBase.h"
#include "StateTreeLinker.h"

UQuestStateTreeSchema::UQuestStateTreeSchema()
{
}

// TConstArrayView<FStateTreeExternalDataDesc> UQuestStateTreeSchema::GetContextDataDescs() const
// {
// 	static const FStateTreeExternalDataDesc Descs[] =
// 	{
// 		FStateTreeExternalDataDesc(UKRQuestComponent::StaticClass(), EStateTreeExternalDataRequirement::Required),
// 		FStateTreeExternalDataDesc(UKRQuestInstance::StaticClass(), EStateTreeExternalDataRequirement::Optional),
// 		FStateTreeExternalDataDesc(FStateTreeStructRef::StaticStruct(), EStateTreeExternalDataRequirement::Optional)
// 	};
//
// 	return Descs;
// }

bool UQuestStateTreeSchema::Link(FStateTreeLinker& Linker)
{
	//Linker.LinkExternalData(SampleIndexHandle);

	return true;
}

bool UQuestStateTreeSchema::IsStructAllowed(const UScriptStruct* InStruct) const
{
	if (!InStruct)
	{
		return false;
	}

	// Quest 전용 노드만 허용 (이름이 "Quest"로 시작하는 노드)
	// 또는 기본 StateTree 노드는 허용
	const FString StructName = InStruct->GetName();

	// Quest 관련 노드
	if (StructName.StartsWith(TEXT("QuestStateTree")) ||
	    StructName.StartsWith(TEXT("FQuestStateTree")))
	{
		return true;
	}

	// 기본 StateTree 노드 (CommonBase 등)
	if (InStruct->IsChildOf(FStateTreeTaskCommonBase::StaticStruct()) ||
	    InStruct->IsChildOf(FStateTreeConditionCommonBase::StaticStruct()) ||
	    InStruct->IsChildOf(FStateTreeEvaluatorCommonBase::StaticStruct()))
	{
		return true;
	}

	return Super::IsStructAllowed(InStruct);
}

bool UQuestStateTreeSchema::IsClassAllowed(const UClass* InClass) const
{
	if (!InClass)
	{
		return false;
	}

	// Quest 관련 클래스만 허용
	const FString ClassName = InClass->GetName();
	if (ClassName.StartsWith(TEXT("Quest")) ||
	    ClassName.StartsWith(TEXT("KRQuest")))
	{
		return true;
	}

	return Super::IsClassAllowed(InClass);
}

bool UQuestStateTreeSchema::IsExternalItemAllowed(const UStruct& InStruct) const
{
	// Quest 관련 클래스만 허용
	const FString ClassName = InStruct.GetName();
	if (ClassName.StartsWith(TEXT("Quest")) ||
		ClassName.StartsWith(TEXT("KRQuest")))
	{
		return true;
	}
	
	return Super::IsExternalItemAllowed(InStruct);
}
