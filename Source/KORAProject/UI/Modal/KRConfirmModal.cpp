// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Modal/KRConfirmModal.h"
#include "CommonTextBlock.h"
#include "CommonButtonBase.h"

void UKRConfirmModal::SetupConfirm(const FText& InMessage)
{
	if (AlertText) AlertText->SetText(InMessage);
}

void UKRConfirmModal::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (YesButton) YesButton->OnClicked().AddUObject(this, &ThisClass::HandleYes);
	if (NoButton) NoButton->OnClicked().AddUObject(this, &ThisClass::HandleNo);

	//SetConsumeInput(ture);
	//SetIsModal(true);
}

void UKRConfirmModal::NativeOnDeactivated()
{
	OnConfirmed.Unbind();
	Super::NativeOnDeactivated();
}

void UKRConfirmModal::HandleYes()
{
	if (OnConfirmed.IsBound()) OnConfirmed.Execute();
	DeactivateWidget();
}

void UKRConfirmModal::HandleNo()
{
	DeactivateWidget();
}
