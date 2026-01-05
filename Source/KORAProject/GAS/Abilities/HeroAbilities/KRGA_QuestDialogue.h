#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "Data/DialogueDataStruct.h"
#include "KRGA_QuestDialogue.generated.h"

class AKRNPCInteractActor;

UCLASS()
class KORAPROJECT_API UKRGA_QuestDialogue : public UKRGameplayAbility
{
	GENERATED_BODY()

public:
	UKRGA_QuestDialogue(const FObjectInitializer& ObjectInitializer);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
	FName DialogueRouteName = "Quest";

private:
	AKRNPCInteractActor* GetNPCFromPlayerController(const FGameplayAbilityActorInfo* ActorInfo) const;
	FDialogueDataStruct LoadDialogueData(int32 DialogueIndex, FGameplayTag NPCTag) const;
	FName MakeAlreadyTalkedKey(int32 QuestIndex) const;
};
