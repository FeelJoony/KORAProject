#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "IndicatorDescriptor.generated.h"

class UUserWidget;

/**
 * Describes an indicator to be displayed in the world or on screen
 */
UCLASS(BlueprintType)
class KORAPROJECT_API UIndicatorDescriptor : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Indicator")
	TObjectPtr<UObject> DataObject;

	UPROPERTY(BlueprintReadWrite, Category = "Indicator")
	TWeakObjectPtr<USceneComponent> ComponentToTrack;

	UPROPERTY(BlueprintReadWrite, Category = "Indicator")
	bool bVisible;

	UPROPERTY(BlueprintReadWrite, Category = "Indicator")
	bool bClampToScreen;

	UPROPERTY(BlueprintReadWrite, Category = "Indicator")
	int32 Priority;

	UPROPERTY(BlueprintReadWrite, Category = "Indicator")
	TSoftClassPtr<UUserWidget> IndicatorWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "Indicator")
	TObjectPtr<UUserWidget> IndicatorWidget;

	UIndicatorDescriptor();

	UFUNCTION(BlueprintCallable, Category = "Indicator")
	FVector IndicatorLocation() const;

	UFUNCTION(BlueprintCallable, Category = "Indicator")
	void SetSceneComponent(USceneComponent* Component);

	UFUNCTION(BlueprintCallable, Category = "Indicator")
	void SetDataObject(UObject* InDataObject);

	UFUNCTION(BlueprintCallable, Category = "Indicator")
	void SetIndicatorClass(const TSoftClassPtr<UUserWidget> InIndicatorWidgetClass)
	{
		IndicatorWidgetClass=InIndicatorWidgetClass;
	}
	
};
