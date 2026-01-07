#include "KRAIStateTree_DebugFormattedText.h"
#include "DrawDebugHelpers.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus UKRAIStateTree_DebugFormattedText::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition)
{
	UWorld* World = Actor ? Actor->GetWorld() : Context.GetWorld();

	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("UKRAIStateTree_DebugFormattedText: No valid World!"));
		return EStateTreeRunStatus::Failed;
	}

	FFormatNamedArguments Args;
	for (const FKRAI_DebugFormatParam& Param : Params)
	{
		const FString KeyString = Param.Key.ToString();

		switch (Param.Type)
		{
			case EKRAI_DebugParamType::Float:
				Args.Add(KeyString,  FFormatArgumentValue(Param.FloatValue));
				break;
				
			case EKRAI_DebugParamType::Int:
				Args.Add(KeyString,  FFormatArgumentValue(Param.IntValue));
				break;
				
			case EKRAI_DebugParamType::String:
				Args.Add(KeyString,  FFormatArgumentValue(FText::FromString(Param.StringValue)));
				break;

			case EKRAI_DebugParamType::Name:
				Args.Add(KeyString,  FFormatArgumentValue(FText::FromName(Param.NameValue)));
				break;
	
			case EKRAI_DebugParamType::Text:
				Args.Add(KeyString,  FFormatArgumentValue(Param.TextValue));
				break;
			case EKRAI_DebugParamType::Bool:
				Args.Add(KeyString,  FFormatArgumentValue(Param.BoolValue));
				break;
		}
	}

	const FText FormattedText = FText::Format(FormatText, Args);
	const FString Message = FormattedText.ToString();

	const FVector DrawLoc = Actor
		? Actor->GetActorLocation()
		: FVector::ZeroVector;

	DrawDebugString(World, DrawLoc, Message, Actor.Get(), FColor::White, 5.f, true);

	if (PrintToScreen && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, Message);
	}

	return EStateTreeRunStatus::Succeeded;
}
