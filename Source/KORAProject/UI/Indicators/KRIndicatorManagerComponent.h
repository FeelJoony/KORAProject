#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KRIndicatorManagerComponent.generated.h"

class UIndicatorDescriptor;
class APlayerController;

UCLASS(BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class KORAPROJECT_API UKRIndicatorManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UKRIndicatorManagerComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Adds an indicator to be managed */
	UFUNCTION(BlueprintCallable, Category = "Indicator")
	void AddIndicator(UIndicatorDescriptor* IndicatorDescriptor);

	/** Removes an indicator from management */
	UFUNCTION(BlueprintCallable, Category = "Indicator")
	void RemoveIndicator(UIndicatorDescriptor* IndicatorDescriptor);

	/** Gets all currently managed indicators */
	UFUNCTION(BlueprintCallable, Category = "Indicator")
	const TArray<UIndicatorDescriptor*>& GetIndicators() const { return Indicators; }

protected:
	virtual void BeginPlay() override;

	/** Array of all managed indicators */
	UPROPERTY()
	TArray<TObjectPtr<UIndicatorDescriptor>> Indicators;

	/** Updates the visibility and position of all indicators */
	void UpdateIndicators(float DeltaTime);

	/** Gets the owning player controller */
	APlayerController* GetOwningPlayerController() const;
};
