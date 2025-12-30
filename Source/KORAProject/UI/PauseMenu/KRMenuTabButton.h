#pragma once

#include "CommonButtonBase.h"
#include "KRMenuTabButton.generated.h"

class UImage;
class UTexture2D;

UENUM(BlueprintType)
enum class EKRPauseMenuTab : uint8
{
	Equipment,
	Inventory,
	SkillTree,
	Settings,
	Quit
};

UCLASS()
class KORAPROJECT_API UKRMenuTabButton : public UCommonButtonBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu") EKRPauseMenuTab TabType = EKRPauseMenuTab::Equipment;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu") FName MenuNameKey;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu") FName RouteName;

protected:
	virtual void SynchronizeProperties() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Menu|Icon")
	TMap<EKRPauseMenuTab, TObjectPtr<UTexture2D>> TabIconMap;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> TabIcon;
};
