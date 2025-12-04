// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonButtonBase.h"
#include "KRMenuTabButton.generated.h"

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
};
