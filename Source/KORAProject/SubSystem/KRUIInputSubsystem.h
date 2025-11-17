// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "Input/UIActionBindingHandle.h"
#include "Delegates/DelegateCombinations.h"
#include "KRUIInputSubsystem.generated.h"

class UCommonActivatableWidget;
enum class EKRUILayer : uint8;

USTRUCT(BlueprintType)
struct FKRUIInputRowCatalog
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Pause = TEXT("Pause");
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Back = TEXT("Back");
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Select = TEXT("Select");
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Unequip = TEXT("Unequip");
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Navigate = TEXT("Navigate");
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Hover = TEXT("Hover");
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Prev = TEXT("Prev");
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Next = TEXT("Next");
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Increase = TEXT("Increase");
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Decrease = TEXT("Decrease");
};

DECLARE_DYNAMIC_DELEGATE(FKRUISimpleDynDelegate);

USTRUCT(BlueprintType)
struct FKRUIBindingHandle // Binding Handle for Blueprint
{
	GENERATED_BODY()
public:
	bool IsValid() const { return Native.IsValid(); }

private:
	FUIActionBindingHandle Native;

	friend class UKRUIInputSubsystem;
};

UCLASS(Config = Game, DefaultConfig)
class KORAPROJECT_API UKRUIInputSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Config, Category = "UIInput") TSoftObjectPtr<UDataTable> InputActionDataTable;
	UPROPERTY(EditDefaultsOnly, Category = "UIInput") FKRUIInputRowCatalog Rows;
	UPROPERTY(EditDefaultsOnly, Category = "UIInput") FGameplayTag UITag = FGameplayTag::RequestGameplayTag(TEXT("Player.Mode.UI"));


	FUIActionBindingHandle BindRow(UCommonActivatableWidget* Widget, FName RowName, FSimpleDelegate Handler, bool bShowInBar = true, bool bConsume = true);
	UFUNCTION(BlueprintCallable, Category = "UIInput", meta = (AutoCreateRefTerm = "Handler,RowName"))
	FKRUIBindingHandle BindRow_BP(UCommonActivatableWidget* Widget, FName RowName, FKRUISimpleDynDelegate Handler, bool bShowInBar = true, bool bConsume = true);
	
	
	UFUNCTION(BlueprintCallable, Category = "UIInput") void BindBackDefault(UCommonActivatableWidget* Widget, FName RouteName = NAME_None);
	UFUNCTION(BlueprintCallable, Category = "UIInput") void UnbindAll(UCommonActivatableWidget* Widget);

	UFUNCTION(BlueprintCallable, Category = "UIMode")
	void EnterUIMode(EMouseCaptureMode Capture = EMouseCaptureMode::NoCapture, bool bShowCursor = true);
	UFUNCTION(BlueprintCallable, Category = "UIMode")
	void ReleaseUIMode();

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	int32 UIModeRefCount = 0;
	TMap<TWeakObjectPtr<UCommonActivatableWidget>, TArray<FUIActionBindingHandle>> Handles;
	 
	EMouseCaptureMode LastCapture = EMouseCaptureMode::NoCapture; // -> UE 5.6 기준으로 어떻게 사용하는지 봐야할듯
	bool bUIModeApplied = false;
	bool bDesiredShowCursor = true;

	FDataTableRowHandle MakeRow(FName RowName) const;
	void EnsureLifecycleHook(UCommonActivatableWidget* Widget);
	void ApplyEnable(EMouseCaptureMode Capture, bool bShowCursor);
	void ApplyDisable();

	bool IsAnyNonGameLayerOpen() const;
	void UpdateUIMode();

	static FSimpleDelegate MakeSimpleDelegateFromDynamic(const FKRUISimpleDynDelegate& Dyn);

	UFUNCTION() void OnRouterRouteOpened(ULocalPlayer* LP, FName Route, EKRUILayer Layer);
	UFUNCTION() void OnRouterRouteClosed(ULocalPlayer* LP, FName Route, EKRUILayer Layer);
};
