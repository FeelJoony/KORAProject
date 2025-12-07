// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/Modules/KRPlayerStatusWidget.h"
#include "GameplayTag/KRItemTypeTag.h"
#include "Player/KRPlayerState.h"
#include "Components/ProgressBar.h"
#include "CommonTextBlock.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/AttributeSets/KRCombatCommonSet.h"
#include "GAS/AttributeSets/KRPlayerAttributeSet.h"

void UKRPlayerStatusWidget::OnHUDInitialized()
{
	Super::OnHUDInitialized();

	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& Subsys = UGameplayMessageSubsystem::Get(World);

		WeaponMessageHandle = Subsys.RegisterListener(
			FKRUIMessageTags::Weapon(),
			this,
			&ThisClass::OnWeaponMessageReceived
		);

		GuardMessageHandle = Subsys.RegisterListener(
			FKRUIMessageTags::Guard(),
			this,
			&ThisClass::OnGuardMessageReceived
		);
	}

	BindToASC();
	InitBarsFromASC();

	UpdateWeaponAmmoDisplay(KRTAG_ITEMTYPE_EQUIP_SWORD, 15, 15);
	BP_OnWeaponEquipped(KRTAG_ITEMTYPE_EQUIP_SWORD, false);
}

void UKRPlayerStatusWidget::NativeDestruct()
{
	UnbindAll();
	Super::NativeDestruct();
}

void UKRPlayerStatusWidget::UnbindAll()
{
	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& Subsys = UGameplayMessageSubsystem::Get(World);
		Subsys.UnregisterListener(WeaponMessageHandle);
		Subsys.UnregisterListener(GuardMessageHandle);

		FTimerManager& TM = World->GetTimerManager();
		TM.ClearTimer(HealthAnimTimerHandle);
		TM.ClearTimer(StaminaAnimTimerHandle);
		TM.ClearTimer(GreyHPAnimTimerHandle);
	}
	if (CachedASC.IsValid())
	{
		if (HealthChangedHandle.IsValid())
		{
			CachedASC->GetGameplayAttributeValueChangeDelegate(
				UKRCombatCommonSet::GetCurrentHealthAttribute()
			).Remove(HealthChangedHandle);
		}
		if (StaminaChangedHandle.IsValid())
		{
			CachedASC->GetGameplayAttributeValueChangeDelegate(
				UKRPlayerAttributeSet::GetCurrentStaminaAttribute()
			).Remove(StaminaChangedHandle);
		}
		if (GreyHPChangedHandle.IsValid())
		{
			CachedASC->GetGameplayAttributeValueChangeDelegate(
				UKRPlayerAttributeSet::GetGreyHPAttribute()
			).Remove(GreyHPChangedHandle);
		}
		if (CoreDriveChangedHandle.IsValid())
		{
			CachedASC->GetGameplayAttributeValueChangeDelegate(
				UKRPlayerAttributeSet::GetCoreDriveAttribute()
			).Remove(CoreDriveChangedHandle);
		}
	}
}

void UKRPlayerStatusWidget::BindToASC()
{
	UAbilitySystemComponent* ASC = nullptr;
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AKRPlayerState* KRPS = PC->GetPlayerState<AKRPlayerState>())
		{
			ASC = KRPS->GetAbilitySystemComponent();
		}
	}

	CachedASC = ASC;

	if (const UKRCombatCommonSet* Combat = ASC->GetSet<UKRCombatCommonSet>())
	{
		CombatAttr = Combat;

		HealthChangedHandle =
			ASC->GetGameplayAttributeValueChangeDelegate(
				UKRCombatCommonSet::GetCurrentHealthAttribute()
			).AddUObject(this, &ThisClass::OnHealthAttributeChanged);
	}

	if (const UKRPlayerAttributeSet* Player = ASC->GetSet<UKRPlayerAttributeSet>())
	{
		PlayerAttr = Player;

		StaminaChangedHandle =
			ASC->GetGameplayAttributeValueChangeDelegate(
				UKRPlayerAttributeSet::GetCurrentStaminaAttribute()
			).AddUObject(this, &ThisClass::OnStaminaAttributeChanged);

		GreyHPChangedHandle =
			ASC->GetGameplayAttributeValueChangeDelegate(
				UKRPlayerAttributeSet::GetGreyHPAttribute()
			).AddUObject(this, &ThisClass::OnGreyHealthAttributeChanged);

		CoreDriveChangedHandle =
			ASC->GetGameplayAttributeValueChangeDelegate(
				UKRPlayerAttributeSet::GetCoreDriveAttribute()
			).AddUObject(this, &ThisClass::OnCoreDriveAttributeChanged);
	}
}

void UKRPlayerStatusWidget::InitBarsFromASC()
{
	float HPPercent = 1.f;
	float GreyPercent = 1.f;
	float StaminaPercent = 1.f;
	float CoreNormalized = 0.f;

	float CurHP = 0.f;
	float MaxHP = 0.f;
	float RecoverableGrey = 0.f;

	if (CombatAttr.IsValid())
	{
		MaxHP = CombatAttr->GetMaxHealth();
		CurHP = CombatAttr->GetCurrentHealth();

		HPPercent = (MaxHP > 0.f) ? CurHP / MaxHP : 0.f;
	}

	if (PlayerAttr.IsValid() && MaxHP > 0.f)
	{
		RecoverableGrey = PlayerAttr->GetGreyHP();

		const float GreyHPPercent = FMath::Clamp(
			(CurHP + RecoverableGrey) / MaxHP,
			HPPercent,
			1.f
		);

		GreyPercent = GreyHPPercent;

		const float MaxStam = PlayerAttr->GetMaxStamina();
		const float CurStam = PlayerAttr->GetCurrentStamina();
		StaminaPercent = (MaxStam > 0.f) ? CurStam / MaxStam : 0.f;

		CoreDriveMax = PlayerAttr->GetMaxCoreDrive();
		CoreDriveValue = PlayerAttr->GetCoreDrive();
		CoreNormalized = (CoreDriveMax > 0.f) ? CoreDriveValue / CoreDriveMax : 0.f;
	}

	if (MainHPBar)  MainHPBar->SetPercent(HPPercent);
	if (GreyHPBar)  GreyHPBar->SetPercent(GreyPercent);
	if (StaminaBar) StaminaBar->SetPercent(StaminaPercent);

	HealthDisplayPercent = HealthTargetPercent = HPPercent;
	GreyHPDisplayPercent = GreyHPTargetPercent = GreyPercent;
	StaminaDisplayPercent = StaminaTargetPercent = StaminaPercent;

	constexpr int32 NumSegments = 3;
	int32 FullSegments = 0;
	float PartialPercent = 0.f;

	if (CoreDriveMax > 0.f && NumSegments > 0)
	{
		const float SegmentValue = CoreDriveMax / NumSegments;
		FullSegments = FMath::FloorToInt(CoreDriveValue / SegmentValue);
		const float CurSegValue = CoreDriveValue - SegmentValue * FullSegments;
		PartialPercent = FMath::Clamp(CurSegValue / SegmentValue, 0.f, 1.f);
	}

	CoreDriveFullSegmentsPrev = FullSegments;
	bIsCoreDriveFull = (CoreNormalized >= 1.f - KINDA_SMALL_NUMBER);

	BP_OnCoreDriveChanged(FullSegments, PartialPercent);
}

void UKRPlayerStatusWidget::OnHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
	if (!CombatAttr.IsValid() || !MainHPBar) return;

	const float MaxHP = CombatAttr->GetMaxHealth();
	if (MaxHP <= 0.f) return;

	const float NewHP = Data.NewValue;
	const float NewHPPercent = FMath::Clamp(NewHP / MaxHP, 0.f, 1.f);

	const float OldHPPercent = MainHPBar->GetPercent();
	const float Delta = NewHPPercent - OldHPPercent;

	HealthAnimTime = (Delta < 0.f) ? 0.15f : 0.25f;
	HealthDisplayPercent = OldHPPercent;
	HealthTargetPercent = NewHPPercent;
	StartHealthAnim();

	if (GreyHPBar && PlayerAttr.IsValid())
	{
		const float RecoverableGrey = PlayerAttr->GetGreyHP();
		const float NewGreyPercent = FMath::Clamp(
			(NewHP + RecoverableGrey) / MaxHP,
			NewHPPercent,
			1.f
		);

		GreyHPDisplayPercent = GreyHPBar->GetPercent();
		GreyHPTargetPercent = NewGreyPercent;
		GreyHPAnimTime = 0.25f;

		StartGreyHPAnim();
	}
}

void UKRPlayerStatusWidget::OnGreyHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
	if (!CombatAttr.IsValid() || !GreyHPBar || !MainHPBar) return;

	const float MaxHP = CombatAttr->GetMaxHealth();
	if (MaxHP <= 0.f) return;

	const float CurHP = CombatAttr->GetCurrentHealth();
	const float RecoverableNew = Data.NewValue;

	const float HPPercent = FMath::Clamp(CurHP / MaxHP, 0.f, 1.f);

	const float NewGreyPercent = FMath::Clamp(
		(CurHP + RecoverableNew) / MaxHP,
		HPPercent,
		1.f
	);

	GreyHPDisplayPercent = GreyHPBar->GetPercent();
	GreyHPTargetPercent = NewGreyPercent;
	GreyHPAnimTime = 0.25f;

	StartGreyHPAnim();
}

void UKRPlayerStatusWidget::OnStaminaAttributeChanged(const FOnAttributeChangeData& Data)
{
	if (!PlayerAttr.IsValid() || !StaminaBar) return;

	const float MaxStam = PlayerAttr->GetMaxStamina();
	if (MaxStam <= 0.f) return;

	const float OldStam = Data.OldValue;
	const float NewStam = Data.NewValue;

	const float OldPct = FMath::Clamp(OldStam / MaxStam, 0.f, 1.f);
	const float NewPct = FMath::Clamp(NewStam / MaxStam, 0.f, 1.f);
	const float Delta = NewPct - OldPct;

	if (Delta < 0.f)
	{
		StaminaBar->SetPercent(NewPct);
		StaminaDisplayPercent = NewPct;
		StaminaTargetPercent = NewPct;
	}
	else if (Delta > 0.f)
	{
		StaminaDisplayPercent = StaminaBar->GetPercent();
		StaminaTargetPercent = NewPct;
		StartStaminaAnim();
	}

	StaminaDisplayPercent = StaminaBar->GetPercent();
	StaminaTargetPercent = NewPct;
	StartStaminaAnim();

	if (NewPct <= KINDA_SMALL_NUMBER)
	{
		BP_OnStaminaDepleted();
	}
}

void UKRPlayerStatusWidget::OnCoreDriveAttributeChanged(const FOnAttributeChangeData& Data)
{
	if (!PlayerAttr.IsValid()) return;

	CoreDriveMax = PlayerAttr->GetMaxCoreDrive();
	CoreDriveValue = Data.NewValue;

	const float Normalized = (CoreDriveMax > 0.f) ? CoreDriveValue / CoreDriveMax : 0.f;

	constexpr int32 NumSegments = 3;
	int32 FullSegments = 0;
	float PartialPercent = 0.f;

	if (CoreDriveMax > 0.f && NumSegments > 0)
	{
		const float SegmentValue = CoreDriveMax / NumSegments;

		FullSegments = FMath::Clamp(
			FMath::FloorToInt(CoreDriveValue / SegmentValue),
			0,
			NumSegments
		);

		float CurSegValue = CoreDriveValue - SegmentValue * FullSegments;
		if (FullSegments >= NumSegments)
		{
			CurSegValue = SegmentValue;
		}

		PartialPercent = FMath::Clamp(CurSegValue / SegmentValue, 0.f, 1.f);
	}

	BP_OnCoreDriveChanged(FullSegments, PartialPercent);

	if (FullSegments > CoreDriveFullSegmentsPrev)
	{
		BP_OnCoreDriveSegmentFilled(FullSegments);
	}
	CoreDriveFullSegmentsPrev = FullSegments;

	const bool bFullNow = (Normalized >= 1.f - KINDA_SMALL_NUMBER);
	if (bFullNow && !bIsCoreDriveFull)
	{
		BP_OnCoreDriveFull();
	}
	bIsCoreDriveFull = bFullNow;
}

void UKRPlayerStatusWidget::OnWeaponMessageReceived(FGameplayTag Channel, const FKRUIMessage_Weapon& Message)
{
	switch (Message.Action)
	{
	case EWeaponMessageAction::Equipped:
	{
		BP_OnWeaponEquipped(KRTAG_ITEMTYPE_EQUIP_SWORD, true);
		UpdateWeaponAmmoDisplay(KRTAG_ITEMTYPE_EQUIP_SWORD, Message.CurrentAmmo, Message.MaxAmmo);
		break;
	}
	case EWeaponMessageAction::Unequipped:
		BP_OnWeaponEquipped(KRTAG_ITEMTYPE_EQUIP_SWORD, false);
		UpdateWeaponAmmoDisplay(KRTAG_ITEMTYPE_EQUIP_SWORD, Message.CurrentAmmo, Message.MaxAmmo);
		break;
	case EWeaponMessageAction::AmmoUpdated:
	{
		if (Message.WeaponTypeTag == KRTAG_ITEMTYPE_EQUIP_SWORD) return;

		CurrentBullet->SetText(FText::AsNumber(Message.CurrentAmmo));
		MaxBullet->SetText(FText::AsNumber(Message.MaxAmmo));
		break;
	}
	case EWeaponMessageAction::Switched:
		UpdateWeaponAmmoDisplay(Message.WeaponTypeTag, Message.CurrentAmmo, Message.MaxAmmo);
		BP_OnWeaponChanged(Message.WeaponTypeTag);
		break;
	}
}

void UKRPlayerStatusWidget::OnGuardMessageReceived(FGameplayTag Channel, const FKRUIMessage_Guard& Message)
{
	if (!IsMessageFromMyActor(Message.TargetActor)) return;

	if (Message.bPerfectGuard)
	{
		BP_OnPerfectGuardSuccess();
	}
	else if (Message.bGuardSuccess)
	{
		BP_OnGuardSuccess();
	}
}

bool UKRPlayerStatusWidget::IsMessageFromMyActor(const TWeakObjectPtr<AActor>& TargetActor) const
{
	if (!TargetActor.IsValid()) return false;

	if (APlayerController* PC = GetOwningPlayer())
	{
		return true;
	}
	return false;
}

void UKRPlayerStatusWidget::StartHealthAnim()
{
	if (!MainHPBar) return;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(HealthAnimTimerHandle);

		HealthDisplayPercent = MainHPBar->GetPercent();
		HealthAnimElapsed = 0.f;

		World->GetTimerManager().SetTimer(
			HealthAnimTimerHandle,
			this,
			&ThisClass::TickHealthAnim,
			0.01f,
			true
		);
	}
}

void UKRPlayerStatusWidget::TickHealthAnim()
{
	if (!MainHPBar) return;

	if (UWorld* World = GetWorld())
	{
		HealthAnimElapsed += World->GetDeltaSeconds();
		const float Alpha = (HealthAnimTime > 0.f)
			? FMath::Clamp(HealthAnimElapsed / HealthAnimTime, 0.f, 1.f)
			: 1.f;

		const float NewPercent = FMath::Lerp(HealthDisplayPercent, HealthTargetPercent, Alpha);
		MainHPBar->SetPercent(NewPercent);

		if (Alpha >= 1.f - KINDA_SMALL_NUMBER)
		{
			World->GetTimerManager().ClearTimer(HealthAnimTimerHandle);
		}
	}
}

void UKRPlayerStatusWidget::StartGreyHPAnim()
{
	if (!GreyHPBar) return;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(GreyHPAnimTimerHandle);

		GreyHPDisplayPercent = GreyHPBar->GetPercent();
		GreyHPAnimElapsed = 0.f;

		World->GetTimerManager().SetTimer(
			GreyHPAnimTimerHandle,
			this,
			&ThisClass::TickGreyHPAnim,
			0.01f,
			true
		);
	}
}

void UKRPlayerStatusWidget::TickGreyHPAnim()
{
	if (!GreyHPBar) return;

	if (UWorld* World = GetWorld())
	{
		GreyHPAnimElapsed += World->GetDeltaSeconds();
		const float Alpha = (GreyHPAnimTime > 0.f)
			? FMath::Clamp(GreyHPAnimElapsed / GreyHPAnimTime, 0.f, 1.f)
			: 1.f;

		const float NewPercent = FMath::Lerp(GreyHPDisplayPercent, GreyHPTargetPercent, Alpha);
		GreyHPDisplayPercent = NewPercent;
		UpdateGreyHPBar(GreyHPDisplayPercent);

		if (Alpha >= 1.f - KINDA_SMALL_NUMBER)
		{
			World->GetTimerManager().ClearTimer(GreyHPAnimTimerHandle);
		}
	}
}

void UKRPlayerStatusWidget::StartStaminaAnim()
{
	if (!StaminaBar) return;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(StaminaAnimTimerHandle);

		StaminaDisplayPercent = StaminaBar->GetPercent();
		StaminaAnimElapsed = 0.f;

		World->GetTimerManager().SetTimer(
			StaminaAnimTimerHandle,
			this,
			&ThisClass::TickStaminaAnim,
			0.01f,
			true
		);
	}
}

void UKRPlayerStatusWidget::TickStaminaAnim()
{
	if (!StaminaBar) return;

	if (UWorld* World = GetWorld())
	{
		StaminaAnimElapsed += World->GetDeltaSeconds();
		const float Alpha = (StaminaAnimTime > 0.f) ? FMath::Clamp(StaminaAnimElapsed / StaminaAnimTime, 0.f, 1.f) : 1.f;

		const float NewPercent = FMath::Lerp(StaminaDisplayPercent, StaminaTargetPercent, Alpha);
		StaminaBar->SetPercent(NewPercent);

		if (Alpha >= 1.f - KINDA_SMALL_NUMBER)
		{
			World->GetTimerManager().ClearTimer(StaminaAnimTimerHandle);
		}
	}
}

void UKRPlayerStatusWidget::UpdateGreyHPBar(float NewValueNormalized)
{
	if (GreyHPBar)
	{
		GreyHPBar->SetPercent(FMath::Clamp(NewValueNormalized, 0.f, 1.f));
	}
}

void UKRPlayerStatusWidget::UpdateWeaponAmmoDisplay(const FGameplayTag& WeaponTypeTag, int32 CurrentAmmo, int32 MaxAmmo)
{
	if (WeaponTypeTag == KRTAG_ITEMTYPE_EQUIP_GUN)
	{
		if (GunAmmoSection)
		{
			GunAmmoSection->SetVisibility(ESlateVisibility::Visible);
		}
		CurrentBullet->SetText(FText::AsNumber(CurrentAmmo));
		MaxBullet->SetText(FText::AsNumber(MaxAmmo));
	}
	else
	{
		if (GunAmmoSection)
		{
			GunAmmoSection->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
