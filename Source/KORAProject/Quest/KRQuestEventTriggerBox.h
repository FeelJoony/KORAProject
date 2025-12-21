#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "GameplayTagContainer.h"
#include "KRQuestEventTriggerBox.generated.h"

USTRUCT(BlueprintType)
struct FQuestEventTriggerBoxMessage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag EventGameplayTag;
};

UCLASS()
class KORAPROJECT_API AKRQuestEventTriggerBox : public ATriggerBox
{
	GENERATED_BODY()

public:
	AKRQuestEventTriggerBox();
	
	virtual void BeginPlay() override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UPROPERTY(EditInstanceOnly, Category = "Quest|EventGameplayTag")
	FGameplayTag EventGameplayTag;
};
