#pragma once

#include "CoreMinimal.h"
#include "InteractableActorBase.h"
#include "KRNPCInteractActor.generated.h"

UCLASS()
class KORAPROJECT_API AKRNPCInteractActor : public AInteractableActorBase
{
	GENERATED_BODY()

public:
	AKRNPCInteractActor();

	UFUNCTION(BlueprintPure, Category = "NPC")
	FGameplayTag GetNPCTag() const { return NPCTag; }

	UFUNCTION(BlueprintPure, Category = "NPC|Dialogue")
	int32 GetCurrentDialogueIndex() const { return CurrentDialogueIndex; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC")
	FGameplayTag NPCTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC|Dialogue")
	int32 CurrentDialogueIndex = -1;
};
