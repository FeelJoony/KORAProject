#include "UI/PauseMenu/KRPauseMenuWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "SubSystem/KRUIRouterSubsystem.h"
#include "SubSystem/KRUIInputSubsystem.h"
#include "UI/Data/KRUIAdapterLibrary.h"
#include "UI/Inventory/KRInventoryMain.h"
#include "UI/KRSlotGridBase.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWidget.h"
#include "UI/KRItemSlotBase.h"
#include "GameplayTag/KRItemTypeTag.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Groups/CommonButtonGroupBase.h"

void UKRPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Initialize MenuTab ButtonGroup
	MenuTabButtonGroup = NewObject<UCommonButtonGroupBase>(this);
	MenuTabButtonGroup->SetSelectionRequired(false);

	BindMenuButton(EquipmentButton);
	BindMenuButton(InventoryButton);
	BindMenuButton(SettingsButton);
	BindMenuButton(QuitButton);

	if (SlotNameWidget)
	{
		SlotNameWidget->OnPrimaryClicked.AddDynamic(this, &ThisClass::HandleSlotNamePrimary);
		SlotNameWidget->OnSecondaryClicked.AddDynamic(this, &ThisClass::HandleSlotNameSecondary);
	}
	if (QuickSlotWidget)
	{
		QuickSlotWidget->bListenGameplayMessages = true;
		QuickSlotWidget->OnSlotHovered.AddDynamic(this, &ThisClass::HandleQuickSlotHovered);
		QuickSlotWidget->OnSlotClicked.AddDynamic(this, &ThisClass::HandleSelect);
		QuickSlotWidget->RefreshFromInventory();
	}

	if (QuickSlotInventoryGrid)
	{
		QuickSlotInventoryGrid->OnSlotClicked.AddDynamic(this, &ThisClass::HandleSelect);
	}
										
	CloseQuickSlotInventory();

	// Initialize navigation system
	InitializeMenuTabArray();
	HandleMenuHovered(EquipmentButton);
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
	if (QuickSlotInventoryGrid)
	{
		QuickSlotInventoryGrid->OnSlotClicked.RemoveDynamic(this, &ThisClass::HandleQuickSlotInventorySelect);
		QuickSlotInventoryGrid->OnSlotClicked.RemoveDynamic(this, &ThisClass::HandleSelect);
	}
	Super::NativeDestruct();
}

void UKRPauseMenuWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	ResetQuickSlotAssignState(true);

	if (auto* InputSubsys = GetOwningLocalPlayer()->GetSubsystem<UKRUIInputSubsystem>())
	{
		InputSubsys->BindBackDefault(this, TEXT("PauseMenu"));
		InputSubsys->BindRow(this, TEXT("Select"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleSelect));
		InputSubsys->BindRow(this, TEXT("Unequip"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleDeselect));;
		InputSubsys->BindRow(this, TEXT("Prev"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveLeft));
		InputSubsys->BindRow(this, TEXT("Next"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveRight));
		InputSubsys->BindRow(this, TEXT("Increase"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveUp));
		InputSubsys->BindRow(this, TEXT("Decrease"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveDown));
	}
	
	SetNavigationContext(EKRPauseNavigationContext::MenuTab);
	UpdateMenuTabSelection(0);
	HandleMenuHovered(EquipmentButton);
}

void UKRPauseMenuWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();

	CloseQuickSlotInventory();
}

void UKRPauseMenuWidget::BindMenuButton(UKRMenuTabButton* Button)
{
	if (!Button) return;

	Button->OnHovered().AddLambda([this, Button]()
	{
		HandleMenuHovered(Button);
	});

	Button->OnClicked().AddLambda([this, Button]()
	{
	   ResetQuickSlotAssignState(true);
		
	   HandleMenuHovered(Button);
		
	   HandleSlotNamePrimary(EKRSlotNameContext::Menu);
	});
}

void UKRPauseMenuWidget::HandleMenuHovered(UKRMenuTabButton* Button)
{
	if (!SlotNameWidget || !Button) return;
	if (bQuickSlotInventoryOpen) return;
	
	SlotNameWidget->SetupForMenu(Button->MenuNameKey);
	CurrentMenuRouteName = Button->RouteName;
	CurrentSlotContext = EKRSlotNameContext::Menu;
	CurrentQuickSlotDir = FGameplayTag();
}

void UKRPauseMenuWidget::HandleQuickSlotHovered(FGameplayTag SlotDir)
{
	if (!SlotNameWidget) return;

	if (bQuickSlotInventoryOpen) return;
	
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
				if (!Router->IsRouteOpen(CurrentMenuRouteName))
				{
					Router->OpenRoute(CurrentMenuRouteName);
				}
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

		OpenQuickSlotInventoryForSlot(CurrentQuickSlotDir);
		break;
	}
	}
}

void UKRPauseMenuWidget::HandleSlotNameSecondary(EKRSlotNameContext Context)
{
	if (Context == EKRSlotNameContext::QuickSlot_HasItem)
	{
		if (!CurrentQuickSlotDir.IsValid()) return;
		
		FKRItemUIData ItemUIData;
		if (!GetQuickItemUIData(CurrentQuickSlotDir, ItemUIData)) return;
		if (!ItemUIData.ItemTag.IsValid()) return;

		if (UWorld* World = GetWorld())
		{
			FKRUIMessage_Confirm Msg;
			Msg.Context = EConfirmContext::QuickSlotRemove;
			Msg.Result = EConfirmResult::Yes;
			Msg.ItemTag = ItemUIData.ItemTag;
			Msg.SlotTag = CurrentQuickSlotDir;
			Msg.Quantity = ItemUIData.Quantity;

			UGameplayMessageSubsystem::Get(World).BroadcastMessage(
				FKRUIMessageTags::Confirm(),
				Msg
			);
		}
	}
}

bool UKRPauseMenuWidget::GetQuickItemUIData(FGameplayTag SlotDir, FKRItemUIData& OutData) const
{
	return UKRUIAdapterLibrary::GetQuickSlotUIData(
		const_cast<UKRPauseMenuWidget*>(this),
		SlotDir,
		OutData
	);
}

void UKRPauseMenuWidget::OpenQuickSlotInventoryForSlot(const FGameplayTag& SlotDir)
{
	bQuickSlotInventoryOpen = true;
	CurrentQuickSlotDir = SlotDir;
	QuickSlotInventoryItemList.Reset();
	UKRUIAdapterLibrary::GetInventoryUIDataFiltered(this, KRTAG_ITEMTYPE_CONSUME, QuickSlotInventoryItemList);

	if (!QuickSlotInventoryGrid)
	{
		return;
	}
	
	QuickSlotInventoryGrid->InitializeItemGrid(QuickSlotInventoryItemList);
	QuickSlotInventoryGrid->SetVisibility(ESlateVisibility::Visible);
	QuickSlotInventoryGrid->SetIsEnabled(true);
	
	QuickSlotInventoryGrid->RebindButtonGroup();
	
	if (QuickSlotWidget)
	{
		QuickSlotWidget->SetIsEnabled(false);
	}
	
	if (QuickSlotInventoryItemList.Num() > 0)
	{
		if (UWorld* World = GetWorld())
		{
			FTimerHandle TempHandle;
			World->GetTimerManager().SetTimerForNextTick([this]()
			{
				if (QuickSlotInventoryGrid)
				{
					QuickSlotInventoryGrid->SelectIndexSafe(0);

					if (UWidget* SelectedWidget = QuickSlotInventoryGrid->GetSelectedWidget())
					{
						SelectedWidget->SetFocus();
					}
					else
					{
						QuickSlotInventoryGrid->SetFocus();
					}
				}
			});
		}
	}
	else
	{
		QuickSlotInventoryGrid->SetFocus();
	}
}

void UKRPauseMenuWidget::CloseQuickSlotInventory()
{
	bQuickSlotInventoryOpen = false;
	if (QuickSlotInventoryGrid)
	{
		QuickSlotInventoryGrid->SetVisibility(ESlateVisibility::Collapsed);
		QuickSlotInventoryGrid->SetIsEnabled(false);
	}
	
	if (QuickSlotWidget)
	{
		QuickSlotWidget->SetIsEnabled(true);
	}
}

void UKRPauseMenuWidget::HandleQuickSlotInventorySelect()
{
	if (!QuickSlotInventoryGrid) return;
	if (!CurrentQuickSlotDir.IsValid()) return;

	const int32 Index = QuickSlotInventoryGrid->GetSelectedIndex();
    if (!QuickSlotInventoryItemList.IsValidIndex(Index)) return;

	const FKRItemUIData& Data = QuickSlotInventoryItemList[Index];
	if (!Data.ItemTag.IsValid()) return;

	if (UWorld* World = GetWorld())
	{
		FKRUIMessage_Confirm Msg;
		Msg.Context = EConfirmContext::QuickSlotAssign;
		Msg.Result = EConfirmResult::Yes;
		Msg.ItemTag = Data.ItemTag;
		Msg.SlotTag = CurrentQuickSlotDir;
		Msg.Quantity = Data.Quantity;

		UGameplayMessageSubsystem::Get(World).BroadcastMessage(
			FKRUIMessageTags::Confirm(),
			Msg
		);
	}

	if (SlotNameWidget)
	{
		FKRItemUIData NewData;
		if (GetQuickItemUIData(CurrentQuickSlotDir, NewData))
		{
			SlotNameWidget->SetupForQuickSlot(NewData.ItemNameKey);
			CurrentSlotContext = EKRSlotNameContext::QuickSlot_HasItem;
		}
		else
		{
			SlotNameWidget->SetupForQuickSlot(NAME_None);
			CurrentSlotContext = EKRSlotNameContext::QuickSlot_Empty;
		}
	}

	CloseQuickSlotInventory();
	
	if (QuickSlotWidget)  
	{
		QuickSlotWidget->SetFocus();
	}
	else
	{
		SetFocus();
	}
}

void UKRPauseMenuWidget::HandleQuickSlotInventoryMove(uint8 DirIdx)
{
	if (!QuickSlotInventoryGrid) return;

	const int32 Cols = QuickSlotInventoryGrid->GetColumnCount();
	const int32 Num = QuickSlotInventoryGrid->GetNumCells();
	if (Cols <= 0 || Num <= 0) return;

	const int32 Cur = QuickSlotInventoryGrid->GetSelectedIndex();
	const int32 Next = StepGridIndex(Cur, DirIdx, Cols, Num);

	if (Next != Cur)
	{
		QuickSlotInventoryGrid->SelectIndexSafe(Next);
		if (UWidget* W = QuickSlotInventoryGrid->GetSelectedWidget())
		{
			W->SetFocus();
		}
	}
}

int32 UKRPauseMenuWidget::StepGridIndex(int32 Cur, uint8 DirIdx, int32 Cols, int32 Num) const
{
	if (Num <= 0 || Cols <= 0)
	{
		return Cur;
	}

	int32 Row = Cur / Cols;
	int32 Col = Cur % Cols;

	switch (DirIdx)
	{
	case 0:  // Left
		Col = (Col > 0) ? (Col - 1) : Col;
		break;
	case 1:  // Right
		Col = (Col < Cols - 1) ? (Col + 1) : Col;
		break;
	case 2:  // Up
		Row = (Row > 0) ? (Row - 1) : Row;
		break;
	case 3:  // Down
	{
		int32 MaxRow = (Num - 1) / Cols;
		Row = (Row < MaxRow) ? (Row + 1) : Row;
		break;
	}
	default:
		break;
	}

	int32 Next = Row * Cols + Col;
	if (Next >= Num) Next = Num - 1;

	return FMath::Clamp(Next, 0, Num - 1);
}

void UKRPauseMenuWidget::HandleSelect()
{
	const uint64 Frame = GFrameCounter;
	if (LastSelectHandledFrame == Frame)
	{
		return;
	}
	LastSelectHandledFrame = Frame;

	if (bQuickSlotInventoryOpen) HandleQuickSlotInventorySelect();
	else HandleSlotNamePrimary(CurrentSlotContext);
}

void UKRPauseMenuWidget::HandleDeselect()
{
	if (!bQuickSlotInventoryOpen)
	{
		if (!CurrentQuickSlotDir.IsValid() && QuickSlotWidget)
		{
			FGameplayTag HoveredSlot = QuickSlotWidget->GetHoveredSlot();
			if (HoveredSlot.IsValid())
			{
				CurrentQuickSlotDir = HoveredSlot;
				
				FKRItemUIData ItemUIData;
				const bool bHasItem = GetQuickItemUIData(HoveredSlot, ItemUIData);
				CurrentSlotContext = bHasItem ? EKRSlotNameContext::QuickSlot_HasItem : EKRSlotNameContext::QuickSlot_Empty;
			}
		}

		HandleSlotNameSecondary(CurrentSlotContext);
	}
}

void UKRPauseMenuWidget::HandleMoveLeft()
{
	if (bQuickSlotInventoryOpen)
	{
		HandleQuickSlotInventoryMove(0);
		return;
	}
	
	if (NavigationContext == EKRPauseNavigationContext::MenuTab)
	{
		int32 NewIndex = FMath::Max(0, CurrentMenuTabIndex - 1);
		if (NewIndex != CurrentMenuTabIndex)
		{
			UpdateMenuTabSelection(NewIndex);
		}
	}
	else if (NavigationContext == EKRPauseNavigationContext::QuickSlot)
	{
		UpdateQuickSlotSelection(FKRUIMessageTags::QuickSlot_West());
		bQuickSlotOnNorth = false;
	}
}

void UKRPauseMenuWidget::HandleMoveRight()
{
	if (bQuickSlotInventoryOpen)
	{
		HandleQuickSlotInventoryMove(1);
		return;
	}
	
	if (NavigationContext == EKRPauseNavigationContext::MenuTab)
	{
		int32 NewIndex = FMath::Min(MenuTabButtons.Num() - 1, CurrentMenuTabIndex + 1);
		if (NewIndex != CurrentMenuTabIndex)
		{
			UpdateMenuTabSelection(NewIndex);
		}
	}

	else if (NavigationContext == EKRPauseNavigationContext::QuickSlot)
	{
		UpdateQuickSlotSelection(FKRUIMessageTags::QuickSlot_East());
		bQuickSlotOnNorth = false;
	}
}

void UKRPauseMenuWidget::HandleMoveUp()
{
	if (bQuickSlotInventoryOpen)
	{
		HandleQuickSlotInventoryMove(2);
		return;
	}
	
	if (NavigationContext == EKRPauseNavigationContext::QuickSlot)
	{
		if (bQuickSlotOnNorth)
		{
			SetNavigationContext(EKRPauseNavigationContext::MenuTab);
			UpdateMenuTabSelection(LastMenuTabIndex);
		}
		else
		{
			UpdateQuickSlotSelection(FKRUIMessageTags::QuickSlot_North());
			bQuickSlotOnNorth = true;
		}
	}
}

void UKRPauseMenuWidget::HandleMoveDown()
{
	if (bQuickSlotInventoryOpen)
	{
		HandleQuickSlotInventoryMove(3);
		return;
	}

	if (NavigationContext == EKRPauseNavigationContext::MenuTab)
	{
		SetNavigationContext(EKRPauseNavigationContext::QuickSlot);
		UpdateQuickSlotSelection(FKRUIMessageTags::QuickSlot_North());
		bQuickSlotOnNorth = true;
	}

	else if (NavigationContext == EKRPauseNavigationContext::QuickSlot)
	{
		if (bQuickSlotOnNorth)
		{
			UpdateQuickSlotSelection(FKRUIMessageTags::QuickSlot_South());
			bQuickSlotOnNorth = false;
		}
		else
		{
			UpdateQuickSlotSelection(FKRUIMessageTags::QuickSlot_North());
			bQuickSlotOnNorth = true;
		}
	}
}

void UKRPauseMenuWidget::ResetQuickSlotAssignState(bool bAlsoCloseUI)
{
	bQuickSlotInventoryOpen = false;
	CurrentQuickSlotDir = FGameplayTag();
	QuickSlotInventoryItemList.Reset();

	if (QuickSlotInventoryGrid)
	{
		QuickSlotInventoryGrid->SelectIndexSafe(INDEX_NONE);

		if (bAlsoCloseUI)
		{
			QuickSlotInventoryGrid->SetVisibility(ESlateVisibility::Collapsed);
			QuickSlotInventoryGrid->SetIsEnabled(false);
		}
	}

	if (QuickSlotWidget)
	{
		QuickSlotWidget->SetIsEnabled(true);
	}
}

void UKRPauseMenuWidget::InitializeMenuTabArray()
{
	MenuTabButtons.Empty();
	MenuTabButtons.Add(EquipmentButton);
	MenuTabButtons.Add(InventoryButton);
	MenuTabButtons.Add(SettingsButton);
	MenuTabButtons.Add(QuitButton);
	
	if (MenuTabButtonGroup)
	{
		for (UKRMenuTabButton* Button : MenuTabButtons)
		{
			if (Button)
			{
				MenuTabButtonGroup->AddWidget(Button);
			}
		}
	}

	CurrentMenuTabIndex = 0;
	LastMenuTabIndex = 0;
}

void UKRPauseMenuWidget::SetNavigationContext(EKRPauseNavigationContext NewContext)
{
	NavigationContext = NewContext;

	if (NavigationContext == EKRPauseNavigationContext::MenuTab)
	{
		if (QuickSlotWidget)
		{
			QuickSlotWidget->ClearAllSelections();
		}
	}
	else if (NavigationContext == EKRPauseNavigationContext::QuickSlot)
	{
		if (MenuTabButtonGroup)
		{
			MenuTabButtonGroup->DeselectAll();
		}
	}
}

void UKRPauseMenuWidget::UpdateMenuTabSelection(int32 NewIndex)
{
	if (!MenuTabButtons.IsValidIndex(NewIndex))
	{
		return;
	}

	CurrentMenuTabIndex = NewIndex;
	LastMenuTabIndex = NewIndex;
	
	if (MenuTabButtonGroup)
	{
		MenuTabButtonGroup->SelectButtonAtIndex(NewIndex);
	}

	if (UKRMenuTabButton* SelectedTab = MenuTabButtons[NewIndex])
	{
		SelectedTab->SetFocus();
		HandleMenuHovered(SelectedTab);
	}
}

void UKRPauseMenuWidget::UpdateQuickSlotSelection(FGameplayTag NewSlot)
{
	if (!QuickSlotWidget || !NewSlot.IsValid())
	{
		return;
	}

	CurrentQuickSlotSelection = NewSlot;
	QuickSlotWidget->FocusSlot(NewSlot);

	HandleQuickSlotHovered(NewSlot);
}