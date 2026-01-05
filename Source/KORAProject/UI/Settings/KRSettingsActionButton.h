#pragma once

#include "CommonButtonBase.h"
#include "KRSettingsActionButton.generated.h"

class UTextBlock;

UENUM(BlueprintType)
enum class EKRSettingsActionType : uint8
{
	Apply,
	Reset,
	Back
};

UCLASS()
class KORAPROJECT_API UKRSettingsActionButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetButtonText(const FText& InText);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetActionType(EKRSettingsActionType InType);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnHovered() override;
	virtual void NativeOnUnhovered() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ButtonTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EKRSettingsActionType ActionType = EKRSettingsActionType::Apply;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FText ButtonText;

	// Style
	UPROPERTY(EditDefaultsOnly, Category = "Settings|Style")
	FLinearColor NormalColor = FLinearColor(0.7f, 0.7f, 0.7f, 1.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Settings|Style")
	FLinearColor HoveredColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Settings|Style")
	FLinearColor DisabledColor = FLinearColor(0.3f, 0.3f, 0.3f, 1.0f);

private:
	void UpdateVisualState();
};
