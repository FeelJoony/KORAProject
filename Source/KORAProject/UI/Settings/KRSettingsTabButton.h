#pragma once

#include "CommonButtonBase.h"
#include "KRSettingsTabButton.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class KORAPROJECT_API UKRSettingsTabButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetTabText(const FText& InText);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	FText GetTabText() const;

protected:
	virtual void NativeOnCurrentTextStyleChanged() override;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Tab")
	TObjectPtr<UTextBlock> TabText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FText ButtonText;

	// Style
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Style")
	FLinearColor NormalColor = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Settings|Style")
	FLinearColor HoveredColor = FLinearColor(0.8f, 0.8f, 0.8f, 1.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Settings|Style")
	FLinearColor SelectedColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	virtual void NativeConstruct() override;
	virtual void NativeOnSelected(bool bBroadcast) override;
	virtual void NativeOnDeselected(bool bBroadcast) override;
	virtual void NativeOnHovered() override;
	virtual void NativeOnUnhovered() override;

private:
	void UpdateVisualState();
};
