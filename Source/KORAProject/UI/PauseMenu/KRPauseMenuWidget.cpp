// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PauseMenu/KRPauseMenuWidget.h"
#include "Kismet/GameplayStatics.h"
#include "SubSystem/KRInventorySubsystem.h"
#include "SubSystem/KRUIRouterSubsystem.h"
#include "SubSystem/KRUIInputSubsystem.h"
#include "UI/Data/KRUIAdapterLibrary.h"
#include "UI/Inventory/KRInventoryMain.h"

void UKRPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InventorySubsystem = GetGameInstance()->GetSubsystem<UKRInventorySubsystem>();

	BindMenuButton(EquipmentButton);
	BindMenuButton(InventoryButton);
	BindMenuButton(SkillTreeButton);
	BindMenuButton(SettingsButton);
	BindMenuButton(QuitButton);

	if (SlotNameWidget)
	{
		SlotNameWidget->OnPrimaryClicked.AddDynamic(this, &ThisClass::HandleSlotNamePrimary);
		SlotNameWidget->OnSecondaryClicked.AddDynamic(this, &ThisClass::HandleSlotNameSecondary);
	}
	if (QuickSlotWidget)
	{
		QuickSlotWidget->bListenGameplayMessages = false;
		QuickSlotWidget->OnSlotHovered.AddDynamic(this, &ThisClass::HandleQuickSlotHovered);
		QuickSlotWidget->RefreshFromInventory();
	}
}

void UKRPauseMenuWidget::NativeDestruct()
{
	if (SlotNameWidget)
	{
		SlotNameWidget->OnPrimaryClicked.RemoveDynamic(this, &ThisClass::HandleSlotNamePrimary);
		SlotNameWidget->OnSecondaryClicked.RemoveDynamic(this, &ThisClass::HandleSlotNameSecondary);
	}
	if (QuickSlotWidget)
	{
		QuickSlotWidget->OnSlotHovered.RemoveDynamic(this, &ThisClass::HandleQuickSlotHovered);
	}

	Super::NativeDestruct();
}

void UKRPauseMenuWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (auto* InputSubsys = GetOwningLocalPlayer()->GetSubsystem<UKRUIInputSubsystem>())
	{
		InputSubsys->BindBackDefault(this, TEXT("PauseMenu"));
		InputSubsys->BindRow(this, TEXT("Select"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleSelect));
		InputSubsys->BindRow(this, TEXT("Prev"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveLeft));
		InputSubsys->BindRow(this, TEXT("Next"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveRight));
		InputSubsys->BindRow(this, TEXT("Increase"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveUp));
		InputSubsys->BindRow(this, TEXT("Decrease"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveDown));
	}

	HandleMenuHovered(EquipmentButton);
}

void UKRPauseMenuWidget::NativeOnDeactivated()
{
	if (auto* InputSubsys = GetOwningLocalPlayer()->GetSubsystem<UKRUIInputSubsystem>())
	{
		InputSubsys->UnbindAll(this);
	}
	Super::NativeOnDeactivated();
}

void UKRPauseMenuWidget::BindMenuButton(UKRMenuTabButton* Button)
{
	if (!Button) return;

	Button->OnHovered().AddLambda([this, Button]()
		{
			HandleMenuHovered(Button);
		});
}

void UKRPauseMenuWidget::HandleMenuHovered(UKRMenuTabButton* Button)
{
	if (!SlotNameWidget || !Button) return;

	SlotNameWidget->SetupForMenu(Button->MenuNameKey);
	CurrentMenuRouteName = Button->RouteName;
	CurrentSlotContext = EKRSlotNameContext::Menu;
	CurrentQuickSlotDir = FGameplayTag();
}

void UKRPauseMenuWidget::HandleQuickSlotHovered(FGameplayTag SlotDir)
{
	if (!SlotNameWidget) return;

	FKRItemUIData ItemUIData;
	const bool bHasItem = GetQuickItemUIData(SlotDir, ItemUIData);

	if (bHasItem)
	{
		SlotNameWidget->SetupForQuickSlot(ItemUIData.ItemNameKey);
		CurrentSlotContext = EKRSlotNameContext::QuickSlot_HasItem;
	}
	else
	{
		SlotNameWidget->SetupForQuickSlot(NAME_None);
		CurrentSlotContext = EKRSlotNameContext::QuickSlot_Empty;
	}

	CurrentQuickSlotDir = SlotDir;
}

void UKRPauseMenuWidget::HandleSlotNamePrimary(EKRSlotNameContext Context)
{
	switch (Context)
	{
	case EKRSlotNameContext::Menu:
	{
		if (CurrentMenuRouteName.IsNone())
		{
			break;
		}

		if (UGameInstance* GI = GetGameInstance())
		{
			if (UKRUIRouterSubsystem* Router = GI->GetSubsystem<UKRUIRouterSubsystem>())
			{
				Router->ToggleRoute(CurrentMenuRouteName);
			}
		}
		break;
	}
	case EKRSlotNameContext::QuickSlot_HasItem:
	case EKRSlotNameContext::QuickSlot_Empty:
	{
		if (!CurrentQuickSlotDir.IsValid())
		{
			break;
		}

		if (UGameInstance* GI = GetGameInstance())
		{
			if (UKRUIRouterSubsystem* Router = GI->GetSubsystem<UKRUIRouterSubsystem>())
			{
				Router->ToggleRoute(TEXT("Inventory"));

				if (UCommonActivatableWidget* Widget = Router->GetActiveOnLayer(TEXT("GameMenu")))
				{
					if (UKRInventoryMain* Inv = Cast<UKRInventoryMain>(Widget))
					{
						Inv->BeginQuickSlotAssign(CurrentQuickSlotDir);
					}
				}
			}
		}
		break;
	}
	}
}

void UKRPauseMenuWidget::HandleSlotNameSecondary(EKRSlotNameContext Context)
{
	if (Context == EKRSlotNameContext::QuickSlot_HasItem)
	{
		// Remove Button
		// InventorySubsystem->ClearQuickSlot(SlotDir)
	}
}

bool UKRPauseMenuWidget::GetQuickItemUIData(FGameplayTag SlotDir, FKRItemUIData& OutData) const
{
	OutData = FKRItemUIData{};
	if (!InventorySubsystem) return false;

	//FGameplayTag ItemTag;
	//if (!InventorySubsystem->GetQuickSlotData(SlotDir, ItemTag)) return false;

	//const UKRInventoryItemInstance* Instance = InventorySubsystem->GetItem(ItemTag);
	//if (!Instance) return false;
	//if (!UKRUIAdapterLibrary::MakeUIDataFromItemInstance(Instance, OutData)) return false;

	return true;
}

void UKRPauseMenuWidget::HandleSelect()
{
	HandleSlotNamePrimary(CurrentSlotContext);
}

void UKRPauseMenuWidget::HandleMoveLeft()
{
}

void UKRPauseMenuWidget::HandleMoveRight()
{
}

void UKRPauseMenuWidget::HandleMoveUp()
{
}

void UKRPauseMenuWidget::HandleMoveDown()
{
}
