#include "InteractionStatics.h"
#include "InteractableTarget.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

UInteractionStatics::UInteractionStatics()
{
}

AActor* UInteractionStatics::GetActorFromInteractableTarget(TScriptInterface<IInteractableTarget> InteractableTarget)
{
	//사용할 때 정의
	return nullptr;
}

void UInteractionStatics::GetInteractableTargetsFromActor(AActor* Actor,
	TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets)
{
	//사용할 때 정의
}

void UInteractionStatics::AppendInteractableTargetsFromOverlapResults(const TArray<FOverlapResult>& OverlapResults,
	TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets)
{
	//사용할 때 정의
}

void UInteractionStatics::AppendInteractableTargetsFromHitResults(const FHitResult& HitResults,
	TArray<TScriptInterface<IInteractableTarget>>& OutInteractableTargets)
{
	//LineTrace Task에서 Hit로직 구현함
}
