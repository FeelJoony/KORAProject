// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Equipment/KREquipmentMain.h"
#include "UI/Equipment/KREquipmentPreviewActor.h"
#include "UI/KRSlotGridBase.h"
#include "UI/KRItemDescriptionBase.h"
#include "UI/Modal/KRConfirmModal.h"
#include "UI/Data/KRUIAdapterLibrary.h"
#include "SubSystem/KRUIRouterSubsystem.h"
#include "SubSystem/KRUIInputSubsystem.h"
#include "GameplayTag/KRItemTypeTag.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Styling/SlateBrush.h"
#include "Materials/MaterialInstanceDynamic.h"

void UKREquipmentMain::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (auto* InputSubsys = GetOwningLocalPlayer()->GetSubsystem<UKRUIInputSubsystem>())
	{
		InputSubsys->BindBackDefault(this, TEXT("Equipment"));
		InputSubsys->BindRow(this, TEXT("Select"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleSelect));
		InputSubsys->BindRow(this, TEXT("Next"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveRight));
		InputSubsys->BindRow(this, TEXT("Prev"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveLeft));
		InputSubsys->BindRow(this, TEXT("Increase"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveUp));
		InputSubsys->BindRow(this, TEXT("Decrease"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveDown));
	}
	if (EquipCategorySlot)
	{
		EquipCategorySlot->RebindButtonGroup();
	}
	if (EquipInventorySlot)
	{
		EquipInventorySlot->RebindButtonGroup();
	}

	RefreshEquippedCategoryIcons();
	FocusCategory();

	FKRUIMessage_EquipmentUI Msg;
	Msg.bIsOpen = true;
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(FKRUIMessageTags::EquipmentUI(), Msg);

	BindPreviewRenderTarget();
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PreviewBindHandle);
		World->GetTimerManager().SetTimer(
			PreviewBindHandle,
			this,
			&UKREquipmentMain::DelayedBindPreviewRenderTarget,
			0.3f,
			false
		);
	}
}

void UKREquipmentMain::NativeOnDeactivated()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PreviewBindHandle);
	}

	FKRUIMessage_EquipmentUI Msg;
	Msg.bIsOpen = false;
	UGameplayMessageSubsystem::Get(this).BroadcastMessage(FKRUIMessageTags::EquipmentUI(), Msg);

	Super::NativeOnDeactivated();
}

void UKREquipmentMain::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeCategoryOrder();

	if (EquipCategorySlot)
	{
		EquipCategorySlot->bSelectOnHover = true;
		EquipCategorySlot->OnHoverChanged.AddDynamic(this, &ThisClass::OnCategoryHovered);
		EquipCategorySlot->OnSelectionChanged.AddDynamic(this, &ThisClass::OnCategorySelected);
		EquipCategorySlot->OnSlotClicked.AddDynamic(this, &ThisClass::OnCategoryClicked);
	}
	if (EquipInventorySlot)
	{
		EquipInventorySlot->OnHoverChanged.AddDynamic(this, &ThisClass::OnInventoryHovered);
		EquipInventorySlot->OnSelectionChanged.AddDynamic(this, &ThisClass::OnInventorySelected);
		EquipInventorySlot->OnSlotClicked.AddDynamic(this, &ThisClass::OnInventoryClicked);
	}

	RefreshEquippedCategoryIcons();

	// Find first non-empty slot for initial selection
	ActiveCategoryIndex = FindFirstNonEmptySlot();

	if (EquipCategorySlot)
	{
		EquipCategorySlot->SelectIndexSafe(ActiveCategoryIndex);
	}
	FocusCategory();
	
	HideInventorySlot();

	EquipMessageHandle = UGameplayMessageSubsystem::Get(this).RegisterListener(
			FKRUIMessageTags::EquipSlot(),
			this,
			&ThisClass::HandleEquipSlotChanged);

	ConfirmMessageHandle = UGameplayMessageSubsystem::Get(this).RegisterListener(
			FKRUIMessageTags::Confirm(),
			this,
			&ThisClass::HandleConfirmResult);
}

void UKREquipmentMain::NativeDestruct()
{
	if (EquipCategorySlot)
	{
		EquipCategorySlot->OnSelectionChanged.RemoveAll(this);
		EquipCategorySlot->OnHoverChanged.RemoveAll(this);
		EquipCategorySlot->OnSlotClicked.RemoveAll(this);
	}
	if (EquipInventorySlot)
	{
		EquipInventorySlot->OnSelectionChanged.RemoveAll(this);
		EquipInventorySlot->OnHoverChanged.RemoveAll(this);
		EquipInventorySlot->OnSlotClicked.RemoveAll(this);
	}

	if (EquipMessageHandle.IsValid())
	{
		UGameplayMessageSubsystem::Get(this).UnregisterListener(EquipMessageHandle);
	}
	if (ConfirmMessageHandle.IsValid())
	{
		UGameplayMessageSubsystem::Get(this).UnregisterListener(ConfirmMessageHandle);
	}

	Super::NativeDestruct();
}

void UKREquipmentMain::InitializeCategoryOrder()
{
	CategorySlotOrder = {
	KRTAG_ITEMTYPE_EQUIP_SWORDMODULE,
	KRTAG_ITEMTYPE_EQUIP_SWORD,
	KRTAG_ITEMTYPE_EQUIP_GUNMODULE,
	KRTAG_ITEMTYPE_EQUIP_GUN,
	KRTAG_ITEMTYPE_EQUIP_HEAD,
	KRTAG_ITEMTYPE_EQUIP_COSTUME
	};

	LastInventoryIndexPerCategory.SetNum(CategorySlotOrder.Num());
	for (int32& V : LastInventoryIndexPerCategory) V = 0;
}

void UKREquipmentMain::RefreshEquippedCategoryIcons()
{
	if (!EquipCategorySlot) return;

	TArray<FKRItemUIData> Equipped;
	UKRUIAdapterLibrary::GetEquippedCategoryUIData(this, CategorySlotOrder, Equipped);

	EquipCategorySlot->InitializeItemGrid(Equipped);
	const int32 Safe = FMath::Clamp(ActiveCategoryIndex, 0, CategorySlotOrder.Num() - 1);
	EquipCategorySlot->SelectIndexSafe(Safe);
}

void UKREquipmentMain::FilterAndCacheItems(const FGameplayTag& FilterTag)
{
	UKRUIAdapterLibrary::GetInventoryUIDataFiltered(this, FilterTag, InventorySlotUIData);
}

void UKREquipmentMain::RebuildInventoryUI(const FGameplayTag& FilterTag)
{
	FilterAndCacheItems(FilterTag);
	if (!EquipInventorySlot)
	{
		UE_LOG(LogTemp, Error, TEXT("[Equipment] RebuildInventoryUI - EquipInventorySlot is NULL!"));
		UpdateDescriptionUI(INDEX_NONE);
		return;
	}

	EquipInventorySlot->InitializeItemGrid(InventorySlotUIData);
	if (InventorySlotUIData.Num() <= 0)
	{
		UpdateDescriptionUI(INDEX_NONE);
		return;
	}
}

void UKREquipmentMain::UpdateDescriptionUI(int32 CellIndex)
{
	if (!ModuleDescription) return;

	if (InventorySlotUIData.IsValidIndex(CellIndex))
	{
		ModuleDescription->SetVisibility(ESlateVisibility::HitTestInvisible);
		const FKRItemUIData& D = InventorySlotUIData[CellIndex];
		ModuleDescription->UpdateItemInfo(D.ItemNameKey, D.ItemDescriptionKey, D.ItemIcon);
	}
	else
	{
		ModuleDescription->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UKREquipmentMain::UpdateCategoryDescriptionUI(int32 CategoryIndex)
{
	if (!ModuleDescription) return;

	if (!CategorySlotOrder.IsValidIndex(CategoryIndex))
	{
		ModuleDescription->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	
	FKRItemUIData UIData;
	if (UKRUIAdapterLibrary::GetEquippedSlotUIData(this, CategorySlotOrder[CategoryIndex], UIData))
	{
		ModuleDescription->SetVisibility(ESlateVisibility::HitTestInvisible);
		ModuleDescription->UpdateItemInfo(UIData.ItemNameKey, UIData.ItemDescriptionKey, UIData.ItemIcon);
		return;
	}

	ModuleDescription->SetVisibility(ESlateVisibility::Collapsed);
}

void UKREquipmentMain::FocusCategory()
{
	FocusRegion = EFocusRegion::Category;
	if (!EquipCategorySlot) return;

	HideInventorySlot();

	const int32 Safe = FMath::Clamp(ActiveCategoryIndex, 0, CategorySlotOrder.Num() - 1);
	EquipCategorySlot->HoverIndexSafe(Safe);

	if (UWidget* W = EquipCategorySlot->GetSelectedWidget())
	{
		W->SetFocus();
	}
}

void UKREquipmentMain::FocusInventory(int32 PreferIndex)
{
	FocusRegion = EFocusRegion::Grid;

	if (!EquipInventorySlot)
	{
		UE_LOG(LogTemp, Error, TEXT("[Equipment] FocusInventory - EquipInventorySlot is NULL!"));
		FocusCategory();
		return;
	}

	const int32 Num = EquipInventorySlot->GetNumCells();
	if (Num <= 0)
	{
		FocusCategory();
		return;
	}
	ShowInventorySlot();

	const int32 Safe = FMath::Clamp(PreferIndex, 0, Num - 1);
	EquipInventorySlot->SelectIndexSafe(Safe);

	if (UWidget* W = EquipInventorySlot->GetSelectedWidget())
	{
		W->SetFocus();
	}
}

void UKREquipmentMain::ShowInventorySlot()
{
	if (EquipInventorySlot)
	{
		EquipInventorySlot->SetVisibility(ESlateVisibility::Visible);
	}
	if (EquipCategorySlot)
	{
		EquipCategorySlot->SetIsEnabled(false);
	}
}

void UKREquipmentMain::HideInventorySlot()
{
	if (EquipInventorySlot)
	{
		EquipInventorySlot->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (EquipCategorySlot)
	{
		EquipCategorySlot->SetIsEnabled(true);
	}
}
void UKREquipmentMain::HandleMoveLeft() { HandleMoveInternal(ENavDir::L); }
void UKREquipmentMain::HandleMoveRight() { HandleMoveInternal(ENavDir::R); }
void UKREquipmentMain::HandleMoveUp() { HandleMoveInternal(ENavDir::U); }
void UKREquipmentMain::HandleMoveDown() { HandleMoveInternal(ENavDir::D); }

void UKREquipmentMain::HandleMoveInternal(ENavDir Dir)
{
	if (FocusRegion == EFocusRegion::Category)
	{
		MoveCategory(Dir);
	}
	else
	{
		MoveInventory(Dir);
	}
}

void UKREquipmentMain::HandleSelect()
{
	if (FocusRegion == EFocusRegion::Category)
	{
		const int32 HoverIdx = EquipCategorySlot ? EquipCategorySlot->GetHoveredIndex() : ActiveCategoryIndex;

		if (!CategorySlotOrder.IsValidIndex(HoverIdx))
		{
			UE_LOG(LogTemp, Error, TEXT("[Equipment] HandleSelect - Invalid HoverIdx!"));
			return;
		}

		ActiveCategoryIndex = HoverIdx;

		if (EquipCategorySlot)
		{
			EquipCategorySlot->SelectIndexSafe(HoverIdx);
		}

		RebuildInventoryUI(CategorySlotOrder[ActiveCategoryIndex]);

		// 현재 장착된 아이템을 선택
		const int32 EquippedIdx = FindEquippedItemIndex();
		FocusInventory(EquippedIdx);
		return;
	}
	if (TryEquipSelectedItem())
	{
		FocusRegion = EFocusRegion::Category;
	}
}

bool UKREquipmentMain::TryEquipSelectedItem()
{
	if (!EquipInventorySlot) return false;

	const int32 Sel = EquipInventorySlot->GetSelectedIndex();
	if (!InventorySlotUIData.IsValidIndex(Sel)) return false;

	if (LastInventoryIndexPerCategory.IsValidIndex(ActiveCategoryIndex))
	{
		LastInventoryIndexPerCategory[ActiveCategoryIndex] = Sel;
	}

	const FKRItemUIData& Data = InventorySlotUIData[Sel];

	FKRUIMessage_Confirm Msg;
	Msg.Context = EConfirmContext::Equipment;
	Msg.Result = EConfirmResult::None;
	Msg.ItemTag = Data.ItemTag;

	UGameplayMessageSubsystem::Get(this).BroadcastMessage(FKRUIMessageTags::Confirm(), Msg);

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UKRUIRouterSubsystem* Router = GI->GetSubsystem<UKRUIRouterSubsystem>())
		{
			if (UCommonActivatableWidget* Widget = Router->ToggleRoute(TEXT("Confirm")))
			{
				if (UKRConfirmModal* ConfirmModal = Cast<UKRConfirmModal>(Widget))
				{
					ConfirmModal->SetupConfirm(TEXT("Modal_Equipment"), EConfirmContext::Equipment, Data.ItemTag);
				}
			}
		}
	}
	return true;
}

void UKREquipmentMain::HandleEquipSlotChanged(FGameplayTag Channel, const FKRUIMessage_EquipSlot& Msg)
{
	const int32 Index = CategorySlotOrder.IndexOfByKey(Msg.SlotTag);
	if (!CategorySlotOrder.IsValidIndex(Index)) return;

	FKRItemUIData UI;
	if (UKRUIAdapterLibrary::GetEquippedSlotUIData(this, Msg.SlotTag, UI))
	{
		RefreshEquippedCategoryIcons();
	}
	if (Index == ActiveCategoryIndex)
	{
		HighlightEquippedItemInInventory(true);
	}
}

void UKREquipmentMain::HandleConfirmResult(FGameplayTag Channel, const FKRUIMessage_Confirm& Msg)
{
	if (Msg.Context != EConfirmContext::Equipment) return;
	
	HideInventorySlot();
	FocusCategory();
	if (Msg.Result == EConfirmResult::Yes)
	{
		RefreshEquippedCategoryIcons();
	}
}

void UKREquipmentMain::OnCategorySelected(int32 Index, UKRItemSlotBase* SlotBase)
{
	if (FocusRegion != EFocusRegion::Category)
	{
		return;
	}

	ActiveCategoryIndex = Index;
	UpdateCategoryDescriptionUI(Index);
}

void UKREquipmentMain::OnCategoryHovered(int32 Index, UKRItemSlotBase* SlotBase )
{
}

void UKREquipmentMain::OnInventorySelected(int32 Index, UKRItemSlotBase* SlotBase)
{
	UpdateDescriptionUI(Index);

	if (LastInventoryIndexPerCategory.IsValidIndex(ActiveCategoryIndex))
	{
		LastInventoryIndexPerCategory[ActiveCategoryIndex] = Index;
	}
}

void UKREquipmentMain::OnInventoryHovered(int32 Index, UKRItemSlotBase* SlotBase)
{
	UpdateDescriptionUI(Index);
}

void UKREquipmentMain::OnCategoryClicked()
{
	const int32 HoverIdx = EquipCategorySlot ? EquipCategorySlot->GetHoveredIndex() : ActiveCategoryIndex;
	if (!CategorySlotOrder.IsValidIndex(HoverIdx)) return;

	if (FocusRegion == EFocusRegion::Grid && HoverIdx == ActiveCategoryIndex)
	{
		FocusCategory();
		return;
	}

	ActiveCategoryIndex = HoverIdx;
	if (EquipCategorySlot)
	{
		EquipCategorySlot->SelectIndexSafe(HoverIdx);
	}

	RebuildInventoryUI(CategorySlotOrder[ActiveCategoryIndex]);

	const int32 EquippedIdx = FindEquippedItemIndex();
	FocusInventory(EquippedIdx);
}

void UKREquipmentMain::OnInventoryClicked()
{
	if (FocusRegion == EFocusRegion::Grid)
	{
		HandleSelect();
	}
}

int32 UKREquipmentMain::StepGrid(int32 Cur, ENavDir Dir, int32 Cols, int32 Num) const
{
	if (Cur < 0 || Cols <= 0 || Num <= 0) return 0;

	switch (Dir)
	{
	case ENavDir::L: return (Cur % Cols == 0) ? Cur : Cur - 1;
	case ENavDir::R:
	{
		const bool bAtRight = (Cur % Cols) == Cols - 1 || Cur + 1 >= Num;
		return bAtRight ? Cur : Cur + 1;
	}
	case ENavDir::U: return (Cur - Cols >= 0) ? Cur - Cols : Cur;
	case ENavDir::D: return (Cur + Cols < Num) ? Cur + Cols : Cur;
	default:         return Cur;
	}
}

bool UKREquipmentMain::MoveCategory(ENavDir Dir)
{
	if (!EquipCategorySlot) return false;

	const int32 Cols = EquipCategorySlot->GetColumnCount();
	const int32 Num = EquipCategorySlot->GetNumCells();
	const int32 Start = ActiveCategoryIndex;

	const int32 Next = StepGrid(Start, Dir, Cols, Num);
	if (Next != Start)
	{
		ActiveCategoryIndex = Next;
		EquipCategorySlot->SelectIndexSafe(Next);
		EquipCategorySlot->HoverIndexSafe(Next);
		UpdateCategoryDescriptionUI(Next);
		return true;
	}
	return false;
}

bool UKREquipmentMain::MoveInventory(ENavDir Dir)
{
	if (!EquipInventorySlot) return false;

	const int32 Cur = EquipInventorySlot->GetSelectedIndex();
	const int32 Cols = EquipInventorySlot->GetColumnCount();
	const int32 Num = EquipInventorySlot->GetNumCells();

	if (Dir == ENavDir::L && (Cur % FMath::Max(Cols, 1)) == 0)
	{
		if (LastInventoryIndexPerCategory.IsValidIndex(ActiveCategoryIndex))
		{
			LastInventoryIndexPerCategory[ActiveCategoryIndex] = Cur;
		}
		FocusCategory();
		return true;
	}

	const int32 Next = StepGrid(Cur, Dir, Cols, Num);
	if (Next != Cur)
	{
		EquipInventorySlot->SelectIndexSafe(Next);

		if (LastInventoryIndexPerCategory.IsValidIndex(ActiveCategoryIndex))
		{
			LastInventoryIndexPerCategory[ActiveCategoryIndex] = Next;
		}

		if (UWidget* W = EquipInventorySlot->GetSelectedWidget())
		{
			W->SetFocus();
		}
		return true;
	}
	return false;
}

void UKREquipmentMain::HighlightEquippedItemInInventory(bool bSelect)
{
	FKRItemUIData EquippedUI;
	if (!UKRUIAdapterLibrary::GetEquippedSlotUIData(this, CategorySlotOrder[ActiveCategoryIndex], EquippedUI)) return;

	const int32 Found = InventorySlotUIData.IndexOfByPredicate(
		[&](const FKRItemUIData& D) { return D.ItemTag == EquippedUI.ItemTag; });

	if (Found != INDEX_NONE && EquipInventorySlot)
	{
		if (bSelect)
		{
			EquipInventorySlot->SelectIndexSafe(Found);
		}
		EquipInventorySlot->HoverIndexSafe(Found);
	}
}

bool UKREquipmentMain::IsCategorySlotEmpty(int32 Index) const
{
	if (!CategorySlotOrder.IsValidIndex(Index)) return true;

	FKRItemUIData UIData;
	return !UKRUIAdapterLibrary::GetEquippedSlotUIData(const_cast<UKREquipmentMain*>(this), CategorySlotOrder[Index], UIData);
}

int32 UKREquipmentMain::FindFirstNonEmptySlot() const
{
	for (int32 i = 0; i < CategorySlotOrder.Num(); ++i)
	{
		if (!IsCategorySlotEmpty(i))
		{
			return i;
		}
	}
	return 0;
}

int32 UKREquipmentMain::FindNextNonEmptySlot(int32 Current, ENavDir Dir) const
{
	if (!EquipCategorySlot) return Current;

	const int32 Cols = EquipCategorySlot->GetColumnCount();
	const int32 Num = EquipCategorySlot->GetNumCells();
	if (Cols <= 0 || Num <= 0) return Current;

	int32 Next = StepGrid(Current, Dir, Cols, Num);

	if (Next == Current) return Current;
	if (!IsCategorySlotEmpty(Next))
	{
		return Next;
	}

	int32 SearchIdx = Next;
	const int32 MaxIterations = Num;

	for (int32 i = 0; i < MaxIterations; ++i)
	{
		int32 TryNext = StepGrid(SearchIdx, Dir, Cols, Num);
		if (TryNext == SearchIdx)
		{
			break;
		}

		SearchIdx = TryNext;

		if (!IsCategorySlotEmpty(SearchIdx))
		{
			return SearchIdx;
		}
	}
	return Current;
}

int32 UKREquipmentMain::FindEquippedItemIndex() const
{
	if (!CategorySlotOrder.IsValidIndex(ActiveCategoryIndex)) return 0;

	FKRItemUIData EquippedUI;
	if (!UKRUIAdapterLibrary::GetEquippedSlotUIData(const_cast<UKREquipmentMain*>(this), CategorySlotOrder[ActiveCategoryIndex], EquippedUI))
	{
		return 0;
	}

	const int32 Found = InventorySlotUIData.IndexOfByPredicate(
		[&](const FKRItemUIData& D) { return D.ItemTag == EquippedUI.ItemTag; });

	return Found != INDEX_NONE ? Found : 0;
}

UWidget* UKREquipmentMain::NativeGetDesiredFocusTarget() const
{
	if (EquipCategorySlot && EquipCategorySlot->GetVisibility() == ESlateVisibility::Visible)
	{
		return EquipCategorySlot;
	}
	return Super::NativeGetDesiredFocusTarget();
}

void UKREquipmentMain::DelayedBindPreviewRenderTarget()
{
	BindPreviewRenderTarget();
}

void UKREquipmentMain::BindPreviewRenderTarget()
{
	if (!PreviewImage)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (!CachedPreviewActor.IsValid())
	{
		TArray<AActor*> Found;
		UGameplayStatics::GetAllActorsOfClass(World, AKREquipmentPreviewActor::StaticClass(), Found);
		if (Found.Num() > 0)
		{
			CachedPreviewActor = Cast<AKREquipmentPreviewActor>(Found[0]);
		}
	}

	if (!CachedPreviewActor.IsValid())
	{
		return;
	}

	UTextureRenderTarget2D* RT = CachedPreviewActor->GetActiveRenderTarget();
	if (!RT)
	{
		return;
	}
	
	if (PreviewMID)
	{
		PreviewMID->SetTextureParameterValue(TEXT("RenderTarget"), RT);
		return;
	}

	if (!CachedBaseMaterial)
	{
		if (!PreviewMaterialAsset.IsNull())
		{
			CachedBaseMaterial = PreviewMaterialAsset.LoadSynchronous();
		}
		else if (PreviewImage)
		{
			const FSlateBrush& ExistingBrush = PreviewImage->GetBrush();
			if (UObject* Resource = ExistingBrush.GetResourceObject())
			{
				CachedBaseMaterial = Cast<UMaterialInterface>(Resource);
			}
		}
	}
	
	if (CachedBaseMaterial)
	{
		PreviewMID = UMaterialInstanceDynamic::Create(CachedBaseMaterial, this);
		if (PreviewMID)
		{
			FSlateBrush Brush;
			Brush.SetResourceObject(PreviewMID);
			Brush.ImageSize = FVector2D(RT->SizeX, RT->SizeY);
			Brush.DrawAs = ESlateBrushDrawType::Image;
			Brush.Tiling = ESlateBrushTileType::NoTile;
			PreviewImage->SetBrush(Brush);
			return;
		}
	}
	
	FSlateBrush Brush;
	Brush.SetResourceObject(RT);
	Brush.ImageSize = FVector2D(RT->SizeX, RT->SizeY);
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.Tiling = ESlateBrushTileType::NoTile;
	Brush.ImageType = ESlateBrushImageType::FullColor;
	PreviewImage->SetBrush(Brush);
}