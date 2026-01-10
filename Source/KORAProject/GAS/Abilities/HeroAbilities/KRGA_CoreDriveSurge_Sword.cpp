#include "GAS/Abilities/HeroAbilities/KRGA_CoreDriveSurge_Sword.h"
#include "Data/DataAssets/KRStarDashData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/KRCoreDriveComponent.h"
#include "GAS/Abilities/HeroAbilities/Libraries/KRMotionWarpingLibrary.h"
#include "GAS/Abilities/HeroAbilities/KRGA_LockOn.h"
#include "GAS/AttributeSets/KRCombatCommonSet.h"
#include "GameplayTag/KRAbilityTag.h"
#include "GameplayTag/KREventTag.h"
#include "GameplayTag/KRStateTag.h"
#include "GameplayTag/KRSetByCallerTag.h"
#include "GameplayTag/KRGameplayCueTag.h"

UKRGA_CoreDriveSurge_Sword::UKRGA_CoreDriveSurge_Sword(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FGameplayTagContainer Tags;
	Tags.AddTag(KRTAG_ABILITY_ATTACK_COREDRIVESURGE);
	SetAssetTags(Tags);

	ActivationPolicy = EKRAbilityActivationPolicy::OnInputTriggered;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	DashStartCueTag = KRTAG_GAMEPLAYCUE_STARDASH_START;
	DashHitCueTag = KRTAG_GAMEPLAYCUE_STARDASH_DASH;
	HitCheckEventTag = KRTAG_EVENT_COREDRIVE_DASH_HITCHECK;
}

void UKRGA_CoreDriveSurge_Sword::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UKRCoreDriveComponent* CoreDriveComp = GetCoreDriveComponent();
	if (!CoreDriveComp || !CoreDriveComp->HasEnoughCoreDrive(CoreDriveCost))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CoreDriveComp->ConsumeCoreDrive(CoreDriveCost);
	DisableMovement();
	SetupDashWarping();
	ApplyInvincibility();
	DrawDebugHitBox();
	SetCollisionIgnorePawns(true);
	HitActors.Empty();
	// ExecuteDashStartCue(); // Visuals are now driven by KRTAG_STATE_ACTING_COREDRIVESURGE

	if (!DashMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, DashMontage, 1.0f, NAME_None, false);
	if (MontageTask)
	{
		MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
		MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageCompleted);
		MontageTask->ReadyForActivation();
	}

	SetupHitCheckEventListener();
}

void UKRGA_CoreDriveSurge_Sword::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	RemoveInvincibility();
	SetCollisionIgnorePawns(false);
	EnableMovement();

	if (AActor* AvatarActor = GetAvatarActorFromActorInfo())
	{
		UKRMotionWarpingLibrary::RemoveWarpTarget(AvatarActor, WarpTargetName);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

UKRCoreDriveComponent* UKRGA_CoreDriveSurge_Sword::GetCoreDriveComponent() const
{
	return GetAvatarActorFromActorInfo() ? GetAvatarActorFromActorInfo()->FindComponentByClass<UKRCoreDriveComponent>() : nullptr;
}

void UKRGA_CoreDriveSurge_Sword::ApplyInvincibility()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->AddLooseGameplayTag(KRTAG_STATE_IMMUNE_DAMAGE);
		ASC->AddLooseGameplayTag(KRTAG_STATE_IMMUNE_HITREACTION);
		ASC->AddLooseGameplayTag(KRTAG_STATE_ACTING_COREDRIVE_SURGE);

		float FinalDashDistance = DashDistance;
		if (StarDashData)
		{
			FinalDashDistance = StarDashData->Pattern.OuterRadius;
		}

		// Trigger Gameplay Cue with parameters
		FGameplayCueParameters CueParams;
		CueParams.Location = DashStartLocation;
		CueParams.Normal = (DashEndLocation - DashStartLocation).GetSafeNormal();
		CueParams.RawMagnitude = FinalDashDistance;
		ASC->AddGameplayCue(KRTAG_GAMEPLAYCUE_COREDRIVE_SURGE_LOOP, CueParams);
	}
}

void UKRGA_CoreDriveSurge_Sword::RemoveInvincibility()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RemoveLooseGameplayTag(KRTAG_STATE_IMMUNE_DAMAGE);
		ASC->RemoveLooseGameplayTag(KRTAG_STATE_IMMUNE_HITREACTION);
		ASC->RemoveLooseGameplayTag(KRTAG_STATE_ACTING_COREDRIVE_SURGE);
		ASC->RemoveGameplayCue(KRTAG_GAMEPLAYCUE_COREDRIVE_SURGE_LOOP);
	}
}

void UKRGA_CoreDriveSurge_Sword::SetCollisionIgnorePawns(bool bIgnore)
{
	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UCapsuleComponent* CapsuleComp = Character->GetCapsuleComponent())
		{
			if (bIgnore)
			{
				OriginalPawnResponse = CapsuleComp->GetCollisionResponseToChannel(ECC_Pawn);
				CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
			}
			else
			{
				CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, OriginalPawnResponse);
			}
		}
	}
}

void UKRGA_CoreDriveSurge_Sword::SetupDashWarping()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return;

	DashStartLocation = AvatarActor->GetActorLocation();
	AActor* LockedTarget = UKRGA_LockOn::GetLockedTargetFor(AvatarActor);
	FVector DashDirection = LockedTarget ? UKRMotionWarpingLibrary::GetDirectionToActor(AvatarActor, LockedTarget, true) : AvatarActor->GetActorForwardVector();
	DashDirection.Z = 0;
	DashDirection.Normalize();

	// StarDashData가 있으면 해당 거리를 우선 사용
	float FinalDashDistance = DashDistance;
	if (StarDashData)
	{
		FinalDashDistance = StarDashData->Pattern.OuterRadius;
	}

	DashEndLocation = DashStartLocation + DashDirection * FinalDashDistance;
	DashEndLocation.Z = DashStartLocation.Z;

	UKRMotionWarpingLibrary::SetWarpTargetByDirection(AvatarActor, WarpTargetName, DashDirection, FinalDashDistance, LockedTarget != nullptr);
}

void UKRGA_CoreDriveSurge_Sword::PerformDashDamageFromNotify()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->GetWorld()) return;

	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);

	AvatarActor->GetWorld()->SweepMultiByChannel(HitResults, DashStartLocation, AvatarActor->GetActorLocation(), AvatarActor->GetActorQuat(), ECC_Pawn, FCollisionShape::MakeBox(BoxHalfExtent), QueryParams);

	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && !HitActors.Contains(HitActor))
		{
			if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor))
			{
				if (TargetASC != GetAbilitySystemComponentFromActorInfo())
				{
					ApplyDamageToTarget(HitActor, Hit);
					ExecuteHitCue(Hit.ImpactPoint);
					HitActors.Add(HitActor);
				}
			}
		}
	}
}

void UKRGA_CoreDriveSurge_Sword::ApplyDamageToTarget(AActor* TargetActor, const FHitResult& HitResult)
{
	if (!DamageEffectClass) return;

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!SourceASC || !TargetASC) return;

	float BaseAttackPower = SourceASC->GetSet<UKRCombatCommonSet>() ? SourceASC->GetSet<UKRCombatCommonSet>()->GetAttackPower() : 0.f;
	const float FinalDamage = BaseAttackPower * DamageMultiplier;

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());
	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(KRTAG_SETBYCALLER_BASEDAMAGE, FinalDamage);
		FGameplayEffectContextHandle ContextHandle = SpecHandle.Data->GetContext();
		ContextHandle.AddHitResult(HitResult, true);
		SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
	}
}

void UKRGA_CoreDriveSurge_Sword::ExecuteDashStartCue()
{
	if (DashStartCueTag.IsValid() && GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = DashStartLocation;
		CueParams.Normal = (DashEndLocation - DashStartLocation).GetSafeNormal();
		CueParams.RawMagnitude = DashDistance;
		GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(DashStartCueTag, CueParams);
	}
}

void UKRGA_CoreDriveSurge_Sword::ExecuteHitCue(const FVector& HitLocation)
{
	if (DashHitCueTag.IsValid() && GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = HitLocation;
		GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(DashHitCueTag, CueParams);
	}
}

void UKRGA_CoreDriveSurge_Sword::SetupHitCheckEventListener()
{
	if (!HitCheckEventTag.IsValid()) return;

	HitCheckEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, HitCheckEventTag, nullptr, false, true);
	if (HitCheckEventTask)
	{
		HitCheckEventTask->EventReceived.AddDynamic(this, &ThisClass::OnHitCheckEventReceived);
		HitCheckEventTask->ReadyForActivation();
	}
}

void UKRGA_CoreDriveSurge_Sword::OnHitCheckEventReceived(FGameplayEventData Payload)
{
	PerformDashDamageFromNotify();
}

void UKRGA_CoreDriveSurge_Sword::DisableMovement()
{
	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement())
		{
			OriginalGravityScale = MovementComp->GravityScale;
			OriginalMovementMode = MovementComp->MovementMode;
			MovementComp->GravityScale = 0.0f;
			MovementComp->SetMovementMode(MOVE_Flying);
			MovementComp->StopMovementImmediately();
		}
	}
}

void UKRGA_CoreDriveSurge_Sword::EnableMovement()
{
	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement())
		{
			MovementComp->GravityScale = OriginalGravityScale;
			MovementComp->SetMovementMode(OriginalMovementMode);
		}
	}
}

void UKRGA_CoreDriveSurge_Sword::DrawDebugHitBox()
{
	if (!bShowDebugShape) return;
	
	UWorld* World = GetWorld();
	if (!World) return;

	const FVector PathCenter = (DashStartLocation + DashEndLocation) * 0.5f;
	const FVector PathExtent(FVector::Dist(DashStartLocation, DashEndLocation) * 0.5f, BoxHalfExtent.Y, BoxHalfExtent.Z);
	const FVector DashDirection = (DashEndLocation - DashStartLocation).GetSafeNormal();
	const FRotator BoxRotation = DashDirection.Rotation();

	DrawDebugBox(World, PathCenter, PathExtent, FQuat(BoxRotation), FColor::Cyan, false, DebugDrawDuration, 0, 3.0f);
	DrawDebugSphere(World, DashStartLocation, 30.0f, 12, FColor::Green, false, DebugDrawDuration, 0, 2.0f);
	DrawDebugSphere(World, DashEndLocation, 30.0f, 12, FColor::Red, false, DebugDrawDuration, 0, 2.0f);
	DrawDebugDirectionalArrow(World, DashStartLocation, DashEndLocation, 50.0f, FColor::Yellow, false, DebugDrawDuration, 0, 3.0f);
}

void UKRGA_CoreDriveSurge_Sword::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UKRGA_CoreDriveSurge_Sword::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}