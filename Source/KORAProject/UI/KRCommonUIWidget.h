// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonActivatableWidget.h"
#include "Engine/DataTable.h"
#include "Input/CommonUIInputTypes.h"
#include "KRCommonUIWidget.generated.h"


UCLASS()
class KORAPROJECT_API UKRCommonUIWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="UI|Input")
	//TSoftObjectPtr<UDataTable> InputActionDataTable; 추후 SoftObjectPtr로 변경 예정 
	TObjectPtr<UDataTable> InputActionDataTable;

protected:
	virtual void SetupInputBindings() {}

	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

	FDataTableRowHandle MakeRow(FName RowName) const;
	FUIActionBindingHandle BindRow(FName Row, FSimpleDelegate Handler, bool bShowInBar = true, bool bConsume = true);

	void BindBackDefault(FName BackRowName = "Back");
	void UnbindAll();

private:
	void OnBackDefault();

	TArray<FUIActionBindingHandle> InputBindingHandles;
};
