#pragma once

#include "CoreMinimal.h"
#include "InteractionOption.h"
#include "Abilities/GameplayAbility.h"
#include "InteractableTarget.generated.h"

class FInteractionOptionBuilder
{
public:
	FInteractionOptionBuilder(TScriptInterface<IInteractableTarget> InteractableTargetScope, TArray<FInteractionOption>& InteractOptions)
		: Scope(InteractableTargetScope)
		, Options(InteractOptions)
	{
	}

	void AddInteractionOption(const FInteractionOption& Option) const
	{
		FInteractionOption& OptionEntry = Options.Add_GetRef(Option);
		OptionEntry.InteractableTarget = Scope;
	}

private:
	TScriptInterface<IInteractableTarget> Scope;
	TArray<FInteractionOption>& Options;
};



UINTERFACE(MinimalAPI, Blueprintable)
class UInteractableTarget : public UInterface
{
	GENERATED_BODY()
};



class IInteractableTarget
{
	GENERATED_BODY()

public:
	virtual void GatherInteractionOptions(const FInteractionQuery& InteractQuery, FInteractionOptionBuilder& OptionBuilder)=0;

	virtual void CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag, FGameplayEventData& InOutEventData){}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	bool CanInteract(const FInteractionQuery& InteractQuery) const;
};
