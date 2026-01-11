// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Shop/KRShopMain.h"
#include "CommonButtonBase.h"
#include "Groups/CommonButtonGroupBase.h"
#include "SubSystem/KRUIInputSubsystem.h"
#include "SubSystem/KRUIRouterSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UKRShopMain::NativeConstruct()
{
	Super::NativeConstruct();

	Buttons.Empty();

	// Create button group
	ButtonGroup = NewObject<UCommonButtonGroupBase>(this);
	ButtonGroup->SetSelectionRequired(true);
	ButtonGroup->OnHoveredButtonBaseChanged.AddDynamic(this, &ThisClass::HandleButtonHovered);
	ButtonGroup->OnSelectedButtonBaseChanged.AddDynamic(this, &ThisClass::HandleButtonSelected);

	if (BuyButton)
	{
		Buttons.Add(BuyButton);
		ButtonGroup->AddWidget(BuyButton);
		BuyButton->OnClicked().AddUObject(this, &ThisClass::OnBuyClicked);
	}
	if (SellButton)
	{
		Buttons.Add(SellButton);
		ButtonGroup->AddWidget(SellButton);
		SellButton->OnClicked().AddUObject(this, &ThisClass::OnSellClicked);
	}

	CurrentButtonIndex = 0;
}

void UKRShopMain::NativeDestruct()
{
	if (ButtonGroup)
	{
		ButtonGroup->OnHoveredButtonBaseChanged.RemoveAll(this);
		ButtonGroup->OnSelectedButtonBaseChanged.RemoveAll(this);
	}

	if (BuyButton)
	{
		BuyButton->OnClicked().RemoveAll(this);
	}
	if (SellButton)
	{
		SellButton->OnClicked().RemoveAll(this);
	}

	Super::NativeDestruct();
}

void UKRShopMain::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (ULocalPlayer* LP = GetOwningLocalPlayer())
	{
		if (UKRUIInputSubsystem* InputSubsys = LP->GetSubsystem<UKRUIInputSubsystem>())
		{
			InputSubsys->BindBackDefault(this, TEXT("ShopMain"));
			InputSubsys->BindRow(this, TEXT("Next"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleNext));
			InputSubsys->BindRow(this, TEXT("Prev"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandlePrev));
			InputSubsys->BindRow(this, TEXT("Select"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleSelect));
		}
	}

	// Set initial selection to BuyButton (index 0)
	SetHoveredIndex(0);
}

void UKRShopMain::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
}

void UKRShopMain::HandleNext()
{
	if (Buttons.Num() == 0) return;

	int32 NewIndex = CurrentButtonIndex + 1;
	if (NewIndex < Buttons.Num())
	{
		SetHoveredIndex(NewIndex);
	}
}

void UKRShopMain::HandlePrev()
{
	if (Buttons.Num() == 0) return;

	int32 NewIndex = CurrentButtonIndex - 1;
	if (NewIndex >= 0)
	{
		SetHoveredIndex(NewIndex);
	}
}

void UKRShopMain::HandleSelect()
{
	if (!Buttons.IsValidIndex(CurrentButtonIndex)) return;

	UCommonButtonBase* SelectedButton = Buttons[CurrentButtonIndex];
	if (SelectedButton)
	{
		if (SelectedButton == BuyButton)
		{
			OnBuyClicked();
		}
		else if (SelectedButton == SellButton)
		{
			OnSellClicked();
		}
	}
}

void UKRShopMain::SetHoveredIndex(int32 NewIndex)
{
	if (!Buttons.IsValidIndex(NewIndex)) return;
	if (!ButtonGroup) return;

	CurrentButtonIndex = NewIndex;
	ButtonGroup->SelectButtonAtIndex(NewIndex);

	if (UCommonButtonBase* Button = Buttons[NewIndex])
	{
		Button->SetFocus();
	}
}

void UKRShopMain::HandleButtonHovered(UCommonButtonBase* Button, int32 Index)
{
	if (!Buttons.IsValidIndex(Index)) return;

	CurrentButtonIndex = Index;
	ButtonGroup->SelectButtonAtIndex(Index);
}

void UKRShopMain::HandleButtonSelected(UCommonButtonBase* Button, int32 Index)
{
	if (!Buttons.IsValidIndex(Index)) return;

	CurrentButtonIndex = Index;
}

void UKRShopMain::OnBuyClicked()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UKRUIRouterSubsystem* Router = GI->GetSubsystem<UKRUIRouterSubsystem>())
		{
			Router->ToggleRoute(TEXT("ShopBuy"));
		}
	}
}

void UKRShopMain::OnSellClicked()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UKRUIRouterSubsystem* Router = GI->GetSubsystem<UKRUIRouterSubsystem>())
		{
			Router->ToggleRoute(TEXT("ShopSell"));
		}
	}
}
