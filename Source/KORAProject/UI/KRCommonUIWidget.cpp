// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/KRCommonUIWidget.h"

void UKRCommonUIWidget::NativeOnActivated()
{
	Super::NativeOnActivated();
	//SetIsModal(true);
	//SetConsumeInput(true);
	SetupInputBindings();
}

void UKRCommonUIWidget::NativeOnDeactivated()
{
	UnbindAll();
	Super::NativeOnDeactivated();
}

FDataTableRowHandle UKRCommonUIWidget::MakeRow(FName RowName) const
{
	FDataTableRowHandle RowHandle;
	RowHandle.DataTable = InputActionDataTable;
	RowHandle.RowName = RowName;
	return RowHandle;
}

FUIActionBindingHandle UKRCommonUIWidget::BindRow(FName Row, FSimpleDelegate Handler, bool bShowInBar, bool bConsume)
{
	FBindUIActionArgs Args(MakeRow(Row), bShowInBar, Handler);
	Args.bConsumeInput = bConsume;
	FUIActionBindingHandle RowHandle = RegisterUIActionBinding(Args);
	InputBindingHandles.Add(RowHandle);
	return RowHandle;
}

void UKRCommonUIWidget::BindBackDefault(FName BackRowName)
{
	BindRow(BackRowName, FSimpleDelegate::CreateUObject(this, &ThisClass::DeactivateWidget));
}

void UKRCommonUIWidget::UnbindAll()
{
	for (FUIActionBindingHandle& Handle : InputBindingHandles)
	{
		if (Handle.IsValid())
		{
			Handle.Unregister();
		}
	}
	InputBindingHandles.Reset();
}

void UKRCommonUIWidget::OnBackDefault()
{
	DeactivateWidget();
}
