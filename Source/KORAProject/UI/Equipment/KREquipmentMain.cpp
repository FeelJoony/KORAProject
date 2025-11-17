// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Equipment/KREquipmentMain.h"
#include "UI/KRSlotGridBase.h"
#include "UI/KRItemDescriptionBase.h"
#include "UI/Data/KRUIAdapterLibrary.h"
#include "UI/Data/KRItemUIData.h"
#include "SubSystem/KRUIInputSubsystem.h"

#include "CommonButtonBase.h"
#include "Groups/CommonButtonGroupBase.h"
#include "GameplayTagsManager.h"
/*
#include "Inventory/KRInventoryComponent.h"   // 인벤토리 컴포넌트 받고 수정
#include "Item/KRBaseItem.h"                 // 아이템 받고 수정
*/

// 필요 함수 리스트 !! 
/* 현재 위젯 소유자 Pawn에서 인벤토리 컴포넌트 가져오는 함수 */
/* 테그 매칭으로 아이템 가져와서 UIData로 변환하는 함수*/

void UKREquipmentMain::NativeOnActivated()
{
	Super::NativeOnActivated();

	UE_LOG(LogTemp, Log, TEXT("NativeOnActivated... Equipment"));
	if (auto* InputSubsys = GetOwningLocalPlayer()->GetSubsystem<UKRUIInputSubsystem>())
	{
		InputSubsys->BindBackDefault(this, TEXT("Equipment"));
		InputSubsys->BindRow(this, TEXT("Select"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleSelect));
		InputSubsys->BindRow(this, TEXT("Next"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveRight));
		InputSubsys->BindRow(this, TEXT("Prev"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveLeft));
		InputSubsys->BindRow(this, TEXT("Increase"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveUp));
		InputSubsys->BindRow(this, TEXT("Decrease"), FSimpleDelegate::CreateUObject(this, &ThisClass::HandleMoveDown));
	}
}

void UKREquipmentMain::NativeOnDeactivated()
{
	UE_LOG(LogTemp, Log, TEXT("NativeOnDeactivated... Equipment"));
	//if (auto* InputSubsys = GetOwningLocalPlayer()->GetSubsystem<UKRUIInputSubsystem>())
	//{
	//}
	Super::NativeOnDeactivated();
}

void UKREquipmentMain::NativeConstruct()
{
	if (SwordModuleSelect)	SwordModuleSelect->OnClicked().AddUObject(this, &UKREquipmentMain::OnClickSwordModule);
	if (SwordSkinSelect)		SwordSkinSelect->OnClicked().AddUObject(this, &UKREquipmentMain::OnClickSwordSkin);
	if (GunModuleSelect)		GunModuleSelect->OnClicked().AddUObject(this, &UKREquipmentMain::OnClickGunModule);
	if (GunSkinSelect)		GunSkinSelect->OnClicked().AddUObject(this, &UKREquipmentMain::OnClickGunSkin);
	if (HeadColorSelect)		HeadColorSelect->OnClicked().AddUObject(this, &UKREquipmentMain::OnClickHeadColor);
	if (DressColorSelect)	DressColorSelect->OnClicked().AddUObject(this, &UKREquipmentMain::OnClickDressColor);

	//BindInventoryEvents();
	BuildCategoryGroup();
	FocusCategory();
	//OnClickSwordModule();
}

void UKREquipmentMain::NativeDestruct()
{
	Super::NativeDestruct();
}

void UKREquipmentMain::OnClickSwordModule() { RebuildByTag(TEXT("ItemType.Equip.SwordModule")); }
void UKREquipmentMain::OnClickSwordSkin() { RebuildByTag(TEXT("ItemType.Equip.Sword")); }
void UKREquipmentMain::OnClickGunModule() { RebuildByTag(TEXT("ItemType.Equip.GunModule")); }
void UKREquipmentMain::OnClickGunSkin() { RebuildByTag(TEXT("ItemType.Equip.Gun")); }
void UKREquipmentMain::OnClickHeadColor() { RebuildByTag(TEXT("ItemType.Equip.Head")); }
void UKREquipmentMain::OnClickDressColor() { RebuildByTag(TEXT("ItemType.Equip.Costume")); }

void UKREquipmentMain::RebuildByTag(const FName& TagName)
{
	const FGameplayTag Tag = UGameplayTagsManager::Get().RequestGameplayTag(TagName);
	RebuildInventoryUI({ Tag });
}

void UKREquipmentMain::BuildCategoryGroup()
{
	CategoryOrder = { SwordModuleSelect, SwordSkinSelect, GunModuleSelect, GunSkinSelect, HeadColorSelect, DressColorSelect };

	CategoryGroup = NewObject<UCommonButtonGroupBase>(this);
	for (UCommonButtonBase* B : CategoryOrder) if (B) CategoryGroup->AddWidget(B);
	CategoryGroup->SetSelectionRequired(true);
	CategoryGroup->OnSelectedButtonBaseChanged.AddDynamic(this, &ThisClass::HandleCategoryChanged);
	CategoryGroup->SelectButtonAtIndex(0);

	CategoryAdj.SetNum(CategoryOrder.Num());
	auto SetAdj = [&](int i, int L, int R, int U, int D)
		{
			CategoryAdj[i].L = L; CategoryAdj[i].R = R; CategoryAdj[i].U = U; CategoryAdj[i].D = D;
		};
	// 0:(L=0,R=1,U=0,D=2), 1:(0,1,1,3), 2:(2,3,0,4), 3:(2,3,1,4), 4:(4,4,2,5), 5:(5,5,4,5)
	SetAdj(0, 0, 1, 0, 2);
	SetAdj(1, 0, 1, 1, 3);
	SetAdj(2, 2, 3, 0, 4);
	SetAdj(3, 2, 3, 1, 4);
	SetAdj(4, 4, 4, 2, 5);
	SetAdj(5, 5, 5, 4, 5);
}

void UKREquipmentMain::HandleCategoryChanged(UCommonButtonBase* SelectedButton, int32 ButtonIndex)
{
	LastCategoryIndex = ButtonIndex;

	if (SelectedButton == SwordModuleSelect) OnClickSwordModule();
	else if (SelectedButton == SwordSkinSelect)   OnClickSwordSkin();
	else if (SelectedButton == GunModuleSelect)   OnClickGunModule();
	else if (SelectedButton == GunSkinSelect)     OnClickGunSkin();
	else if (SelectedButton == HeadColorSelect)   OnClickHeadColor();
	else if (SelectedButton == DressColorSelect)  OnClickDressColor();
}

void UKREquipmentMain::FocusCategory()
{
	FocusRegion = EFocusRegion::Category;
	if (!CategoryGroup) return;

	if (!CategoryGroup->GetSelectedButtonBase())
	{
		CategoryGroup->SelectButtonAtIndex(FMath::Clamp(LastCategoryIndex, 0, CategoryOrder.Num() - 1));
	}
	
	if (UCommonButtonBase* Btn = CategoryGroup->GetSelectedButtonBase())
	{
		Btn->SetFocus();
	}
}

void UKREquipmentMain::FocusGrid(int32 PreferIndex)
{
	FocusRegion = EFocusRegion::Grid;

	if (!InventorySlot) { FocusCategory(); return; }
	const int32 Num = InventorySlot->GetNumCells();
	if (Num <= 0) { FocusCategory(); return; }

	const int32 Safe = FMath::Clamp(PreferIndex, 0, Num - 1);
	InventorySlot->SelectIndexSafe(Safe);
	if (UWidget* W = InventorySlot->GetSelectedWidget()) W->SetFocus();
}

void UKREquipmentMain::OnGridSlotSelected(int32 CellIndex)
{
	UpdateDescriptionUI(CellIndex);
}

void UKREquipmentMain::RebuildInventoryUI(const TArray<FGameplayTag>& TagsAny)
{
	/* 테그 매칭으로 아이템 가져와서 UIData로 변환하는 함수*/
	if (InventorySlot)
	{
		InventorySlot->InitializeItemGrid(CachedUIData);

		if (CachedUIData.Num() > 0)
		{
			UpdateDescriptionUI(0);
		}
		else
		{
			UpdateDescriptionUI(INDEX_NONE);
		}
	}
}

void UKREquipmentMain::UpdateDescriptionUI(int32 CellIndex)
{
	if (!ModuleDescription) return;

	if (CachedUIData.IsValidIndex(CellIndex))
	{
		ModuleDescription->SetVisibility(ESlateVisibility::HitTestInvisible);
		const FKRItemUIData& D = CachedUIData[CellIndex];
		ModuleDescription->UpdateItemInfo(D.ItemNameKey, D.ItemDescriptionKey, D.ItemIcon);
	}
	else
	{
		ModuleDescription->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UKREquipmentMain::HandleMoveLeft() { HandleMoveInternal(0); }
void UKREquipmentMain::HandleMoveRight() { HandleMoveInternal(1); }
void UKREquipmentMain::HandleMoveUp() { HandleMoveInternal(2); }
void UKREquipmentMain::HandleMoveDown() { HandleMoveInternal(3); }

void UKREquipmentMain::HandleMoveInternal(uint8 DirIdx)
{
	if (FocusRegion == EFocusRegion::Category)
	{
		if (!CategoryGroup) return;
		const int32 Cur = CategoryGroup->GetSelectedButtonIndex();
		const int32 Next = StepCategory(Cur, DirIdx);
		if (Next != Cur)
		{
			CategoryGroup->SelectButtonAtIndex(Next);
			if (UCommonButtonBase* Btn = CategoryGroup->GetSelectedButtonBase()) Btn->SetFocus();
		}
	}
	else // Grid
	{
		MoveGrid(DirIdx);
	}
}

void UKREquipmentMain::HandleSelect()
{
	if (FocusRegion == EFocusRegion::Category)
	{
		FocusGrid(0);
	}
	else
	{
		if (TryEquipSelectedItem())
		{
			FocusCategory();
		}
	}
}

bool UKREquipmentMain::TryEquipSelectedItem()
{
	if (!InventorySlot) return false;
	const int32 Sel = InventorySlot->GetSelectedIndex();
	if (!CachedUIData.IsValidIndex(Sel)) return false;

	// 모달 띄우고 확인되면 Try Equip 진행 

	const FKRItemUIData& Pick = CachedUIData[Sel];
	// Equip manager Compo -> Try Equip 
	return true;
}

int32 UKREquipmentMain::StepCategory(int32 Cur, uint8 DirIdx) const
{
	if (!CategoryAdj.IsValidIndex(Cur)) return Cur;
	const FNavAdj& A = CategoryAdj[Cur];
	int32 Next = Cur;
	switch (DirIdx)
	{
	case 0: Next = A.L; break; // Left
	case 1: Next = A.R; break; // Right
	case 2: Next = A.U; break; // Up
	case 3: Next = A.D; break; // Down
	default: break;
	}
	return (Next >= 0) ? Next : Cur;
}

int32 UKREquipmentMain::StepGrid(int32 Cur, uint8 DirIdx, int32 Cols, int32 Num) const
{
	if (Cur < 0 || Cols <= 0 || Num <= 0) return 0;

	switch (DirIdx)
	{
	case 0: // Left
		return (Cur % Cols == 0) ? Cur : Cur - 1;

	case 1: // Right
	{
		const bool bAtRight = (Cur % Cols) == Cols - 1 || Cur + 1 >= Num;
		return bAtRight ? Cur : Cur + 1;
	}

	case 2: // Up
		return (Cur - Cols >= 0) ? Cur - Cols : Cur;

	case 3: // Down
		return (Cur + Cols < Num) ? Cur + Cols : Cur;

	default:
		return Cur;
	}
}

bool UKREquipmentMain::MoveGrid(uint8 DirIdx)
{
	if (!InventorySlot) return false;

	const int32 Cur = InventorySlot->GetSelectedIndex();
	const int32 Cols = InventorySlot->GetColumnCount();
	const int32 Num = InventorySlot->GetNumCells();

	if (DirIdx == 0 && (Cur % FMath::Max(Cols, 1)) == 0)
	{
		FocusCategory();
		return true;
	}

	const int32 Next = StepGrid(Cur, DirIdx, Cols, Num);
	if (Next != Cur)
	{
		InventorySlot->SelectIndexSafe(Next);
		if (UWidget* W = InventorySlot->GetSelectedWidget()) W->SetFocus();
		UpdateDescriptionUI(Next);
		return true;
	}
	return false;
}
