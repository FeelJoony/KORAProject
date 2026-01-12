#include "GAS/Abilities/HeroAbilities/KRGA_CoreDriveBurst_Sword.h"
#include "Data/DataAssets/KRStarDashData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/KRCoreDriveComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/KRCameraMode.h"
#include "Camera/KRCameraMode_Burst.h"
#include "Components/KRCameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/Abilities/HeroAbilities/KRGA_LockOn.h"
#include "GAS/Abilities/HeroAbilities/Libraries/KRMotionWarpingLibrary.h"
#include "GAS/AttributeSets/KRCombatCommonSet.h"
#include "GameplayTag/KRAbilityTag.h"
#include "GameplayTag/KRStateTag.h"
#include "GameplayTag/KRSetByCallerTag.h"
#include "TimerManager.h"
#include "Engine/OverlapResult.h"
#include "DrawDebugHelpers.h"
#include "Characters/KRHeroCharacter.h"
#include "Components/KRHeroComponent.h"

UKRGA_CoreDriveBurst_Sword::UKRGA_CoreDriveBurst_Sword(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FGameplayTagContainer Tags;
	Tags.AddTag(KRTAG_ABILITY_ATTACK_COREDRIVEBURST);
	SetAssetTags(Tags);

	ActivationPolicy = EKRAbilityActivationPolicy::OnInputTriggered;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	StartupCueTag = FGameplayTag::RequestGameplayTag(FName("GameplayCue.CoreDrive.Burst.Startup"), false);
	AttackCueTag = FGameplayTag::RequestGameplayTag(FName("GameplayCue.CoreDrive.Burst.Attack"), false);
	HitCueTag = FGameplayTag::RequestGameplayTag(FName("GameplayCue.CoreDrive.Burst.Hit"), false);
	FinishCueTag = FGameplayTag::RequestGameplayTag(FName("GameplayCue.CoreDrive.Burst.Finish"), false);
}

void UKRGA_CoreDriveBurst_Sword::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UKRCoreDriveComponent* CoreDriveComp = GetCoreDriveComponent();
	if (!CoreDriveComp || !CoreDriveComp->CanUseCoreDriveBurst())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(KRTAG_STATE_ACTING_UNINTERRUPTIBLE);
	GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(KRTAG_STATE_ACTING_COREDRIVE_BURST);
	
	CoreDriveComp->ConsumeAllCoreDrive();

	if (AActor* AvatarActor = GetAvatarActorFromActorInfo())
	{
		BurstAnchorLocation = AvatarActor->GetActorLocation();
		BurstAnchorRotation = AvatarActor->GetActorRotation();

		if (AbilityCameraModeClass)
		{
			if (UKRHeroComponent* HeroComponent = AvatarActor->FindComponentByClass<UKRHeroComponent>())
			{
				if (AKRHeroCharacter* HeroCharacter = Cast<AKRHeroCharacter>(AvatarActor))
				{
					if (UKRCameraComponent* CameraComp = HeroCharacter->GetCameraComponent())
					{
						// 1. 카메라 업데이트 잠금 (설정 중 튀는 현상 방지)
						CameraComp->SetCameraModeUpdateLocked(true);
	
						// 2. 카메라 모드 직접 Push (즉시 적용)
						CameraComp->PushCameraMode(AbilityCameraModeClass);
	
						// 3. 인스턴스 가져와서 초기화 (필수 단계)
						if (UKRCameraMode* ModeInstance = CameraComp->GetCameraModeInstanceByClass(AbilityCameraModeClass))
						{
							if (UKRCameraMode_Burst* BurstMode = Cast<UKRCameraMode_Burst>(ModeInstance))
							{
								BurstMode->SetupBurstCamera(AvatarActor->GetActorLocation(), AvatarActor->GetActorForwardVector());
								BurstMode->SetPlayerActor(AvatarActor);
							}
						}
					}
				}
			}
		}
	}
	
	SetIgnorePawnCollision(true);
	DisableMovement();
	TransitionToPhase(EKRCoreDriveBurstPhase::Startup);
}

void UKRGA_CoreDriveBurst_Sword::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(KRTAG_STATE_ACTING_UNINTERRUPTIBLE);
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(KRTAG_STATE_ACTING_COREDRIVE_BURST);

	if (LoopCueTag.IsValid())
	{
		GetAbilitySystemComponentFromActorInfo()->RemoveGameplayCue(LoopCueTag);
	}

	StopMultiHitSequence();

	if (AActor* AvatarActor = GetAvatarActorFromActorInfo())
	{
		UKRMotionWarpingLibrary::RemoveWarpTarget(AvatarActor, FName("BurstTarget"));
		
		if (AbilityCameraModeClass)
		{
			if (UKRHeroComponent* HeroComponent = AvatarActor->FindComponentByClass<UKRHeroComponent>())
			{
				if (AKRHeroCharacter* HeroCharacter = Cast<AKRHeroCharacter>(AvatarActor))
				{
					if (UKRCameraComponent* CameraComp = HeroCharacter->GetCameraComponent())
					{
						CameraComp->RemoveCameraMode(AbilityCameraModeClass);
						CameraComp->SetCameraModeUpdateLocked(false);
					}
				}
			}
		}
	}

	// 현재 페이즈에 따라 정리
	switch (CurrentPhase)
	{
	case EKRCoreDriveBurstPhase::Startup:
		RemoveHyperArmor();
		break;
	case EKRCoreDriveBurstPhase::Attack:
		RemoveFullInvincibility();
		break;
	default:
		break;
	}

	SetIgnorePawnCollision(false);
	EnableMovement();
	CurrentPhase = EKRCoreDriveBurstPhase::None;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

UKRCoreDriveComponent* UKRGA_CoreDriveBurst_Sword::GetCoreDriveComponent() const
{
	return GetAvatarActorFromActorInfo() ? GetAvatarActorFromActorInfo()->FindComponentByClass<UKRCoreDriveComponent>() : nullptr;
}

void UKRGA_CoreDriveBurst_Sword::TransitionToPhase(EKRCoreDriveBurstPhase NewPhase)
{
	switch (CurrentPhase)
	{
	case EKRCoreDriveBurstPhase::Startup:
		RemoveHyperArmor();
		break;
	case EKRCoreDriveBurstPhase::Attack:
		StopMultiHitSequence();
		RemoveFullInvincibility();
		break;
	default:
		break;
	}

	CurrentPhase = NewPhase;

	switch (NewPhase)
	{
	case EKRCoreDriveBurstPhase::Startup:
		StartStartupPhase();
		break;
	case EKRCoreDriveBurstPhase::Attack:
		StartAttackPhase();
		break;
	case EKRCoreDriveBurstPhase::Finished:
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		break;
	default:
		break;
	}
}

void UKRGA_CoreDriveBurst_Sword::StartStartupPhase()
{
	ApplyHyperArmor();
	ExecuteCue(StartupCueTag);

	if (StartupMontage)
	{
		PlayMontageWithCallback(StartupMontage, &ThisClass::OnStartupMontageCompleted);
	}
	else
	{
		TransitionToPhase(EKRCoreDriveBurstPhase::Attack);
	}
}

void UKRGA_CoreDriveBurst_Sword::StartAttackPhase()
{
	ApplyFullInvincibility();
	ExecuteCue(AttackCueTag);

	if (AActor* AvatarActor = GetAvatarActorFromActorInfo())
	{
		if (AActor* LockedTarget = UKRGA_LockOn::GetLockedTargetFor(AvatarActor))
		{
			FVector DirectionToTarget = UKRMotionWarpingLibrary::GetDirectionToActor(AvatarActor, LockedTarget, true);
			FRotator TargetRotation = UKRMotionWarpingLibrary::GetRotationFromDirection(DirectionToTarget);
			UKRMotionWarpingLibrary::SetWarpTargetRotation(AvatarActor, FName("BurstTarget"), TargetRotation);
			BurstAnchorRotation = TargetRotation; 
		}
	}

	CalculateStarPoints();

	if (LoopCueTag.IsValid())
	{
		// GCN이 Ability를 찾을 수 있도록 함
		FGameplayCueParameters CueParams;
		CueParams.Instigator = GetAvatarActorFromActorInfo();
		CueParams.SourceObject = this; // Ability Instance를 SourceObject로 전달 (EffectCauser는 Actor 타입이라 불가)
		GetAbilitySystemComponentFromActorInfo()->AddGameplayCue(LoopCueTag, CueParams);
	}

	CurrentHitIndex = 0;
	StartMultiHitSequence();

	if (AttackMontage)
	{
		PlayMontageWithCallback(AttackMontage, &ThisClass::OnAttackMontageCompleted);
	}
}

void UKRGA_CoreDriveBurst_Sword::ApplyHyperArmor()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->AddLooseGameplayTag(KRTAG_STATE_IMMUNE_HITREACTION);
	}
}

void UKRGA_CoreDriveBurst_Sword::RemoveHyperArmor()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RemoveLooseGameplayTag(KRTAG_STATE_IMMUNE_HITREACTION);
	}
}

void UKRGA_CoreDriveBurst_Sword::ApplyFullInvincibility()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->AddLooseGameplayTag(KRTAG_STATE_IMMUNE_DAMAGE);
		ASC->AddLooseGameplayTag(KRTAG_STATE_IMMUNE_HITREACTION);
	}
}

void UKRGA_CoreDriveBurst_Sword::RemoveFullInvincibility()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RemoveLooseGameplayTag(KRTAG_STATE_IMMUNE_DAMAGE);
		ASC->RemoveLooseGameplayTag(KRTAG_STATE_IMMUNE_HITREACTION);
	}
}

void UKRGA_CoreDriveBurst_Sword::StartMultiHitSequence()
{
	if (AActor* AvatarActor = GetAvatarActorFromActorInfo())
	{
		if (UWorld* World = AvatarActor->GetWorld())
		{
			World->GetTimerManager().SetTimer(MultiHitTimerHandle, this, &ThisClass::PerformMultiHit, AttackConfig.HitInterval, true, 0.0f);
		}
	}
}

void UKRGA_CoreDriveBurst_Sword::StopMultiHitSequence()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(MultiHitTimerHandle);
		World->GetTimerManager().ClearTimer(FinalHitTimerHandle);
	}
}

void UKRGA_CoreDriveBurst_Sword::PerformMultiHit()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->GetWorld()) return;

	HitActorsThisHit.Empty();
	FVector Origin = BurstAnchorLocation + BurstAnchorRotation.Vector() * AttackConfig.ForwardOffset;
	
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);

	if (AttackConfig.AttackShape == EKRAttackShapeType::Sphere)
	{
		AvatarActor->GetWorld()->OverlapMultiByChannel(OverlapResults, Origin, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(AttackConfig.SphereRadius), QueryParams);
	}
	else if (AttackConfig.AttackShape == EKRAttackShapeType::Box)
	{
		AvatarActor->GetWorld()->OverlapMultiByChannel(OverlapResults, Origin, BurstAnchorRotation.Quaternion(), ECC_Pawn, FCollisionShape::MakeBox(AttackConfig.BoxExtent), QueryParams);
	}

	//DrawDebugAttackShape();

	for (const FOverlapResult& Overlap : OverlapResults)
	{
		AActor* HitActor = Overlap.GetActor();
		if (HitActor && !HitActorsThisHit.Contains(HitActor))
		{
			if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor))
			{
				if (TargetASC != GetAbilitySystemComponentFromActorInfo())
				{
					ApplyDamageToTarget(HitActor, AttackConfig.DamageMultiplierPerHit);
					ExecuteCueAtLocation(HitCueTag, HitActor->GetActorLocation());
					HitActorsThisHit.Add(HitActor);
				}
			}
		}
	}

	// BurstCueTag 실행 로직 제거 (Loop GCN에서 통합 처리)
	
	CurrentHitIndex++;

	// 다음 공격이 마지막이면 타이머를 중지하고 FinalHitDelay만큼 대기
	if (CurrentHitIndex >= (AttackConfig.HitCount - 1))
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(MultiHitTimerHandle);
		}
		ScheduleFinalHit();
	}
}

void UKRGA_CoreDriveBurst_Sword::ScheduleFinalHit()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(FinalHitTimerHandle, this, &ThisClass::PerformFinalHit, AttackConfig.FinalHitDelay, false);
	}
}

void UKRGA_CoreDriveBurst_Sword::PerformFinalHit()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->GetWorld()) return;

	HitActorsThisHit.Empty();
	FVector Origin = BurstAnchorLocation + BurstAnchorRotation.Vector() * AttackConfig.ForwardOffset;

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);
	
	if (AttackConfig.AttackShape == EKRAttackShapeType::Sphere)
	{
		AvatarActor->GetWorld()->OverlapMultiByChannel(OverlapResults, Origin, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(AttackConfig.SphereRadius * 1.5f), QueryParams);
	}
	else if (AttackConfig.AttackShape == EKRAttackShapeType::Box)
	{
		AvatarActor->GetWorld()->OverlapMultiByChannel(OverlapResults, Origin, BurstAnchorRotation.Quaternion(), ECC_Pawn, FCollisionShape::MakeBox(AttackConfig.BoxExtent * 1.5f), QueryParams);
	}

	//DrawDebugAttackShape();

	for (const FOverlapResult& Overlap : OverlapResults)
	{
		if (AActor* HitActor = Overlap.GetActor())
		{
			if (!HitActorsThisHit.Contains(HitActor))
			{
				if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor))
				{
					if (TargetASC != GetAbilitySystemComponentFromActorInfo())
					{
						ApplyDamageToTarget(HitActor, AttackConfig.FinalHitDamageMultiplier);
						ExecuteCueAtLocation(HitCueTag, HitActor->GetActorLocation());
						HitActorsThisHit.Add(HitActor);
					}
				}
			}
		}
	}

	// Finish Cue 실행 (Lightning)
	FGameplayTag CueTag = (StarDashData && StarDashData->Effects.SlashLineEffectTag.IsValid()) 
		? StarDashData->Effects.SlashLineEffectTag // GCN Hit 하나로 처리하되 Magnitude로 구분
		: BurstCueTag;

	if (CueTag.IsValid())
	{
		if (AvatarActor)
		{
			// 피니시 이펙트 위치 계산 (중심점 + 오프셋)
			FVector Center = AvatarActor->GetActorLocation() + (AvatarActor->GetActorForwardVector() * AttackConfig.StarCenterDistance);
			if (StarDashData)
			{
				Center = BurstAnchorLocation + (BurstAnchorRotation.Vector() * AttackConfig.StarCenterDistance);
			}
			
			FVector TargetLoc = Center + AttackConfig.FinisherLocationOffset;

			FGameplayCueParameters CueParams;
			CueParams.Location = AvatarActor->GetActorLocation(); // Start (Player)
			CueParams.EffectContext = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
			CueParams.EffectContext.AddOrigin(TargetLoc); // End (Target)
			CueParams.RawMagnitude = 1.0f; // Finisher
			CueParams.SourceObject = StarDashData; // Pass DataAsset to GCN

			GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(CueTag, CueParams);
		}
	}

	CurrentHitIndex++;
}

void UKRGA_CoreDriveBurst_Sword::ApplyDamageToTarget(AActor* TargetActor, float DamageMultiplier)
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
		SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
	}
}

void UKRGA_CoreDriveBurst_Sword::ExecuteCue(const FGameplayTag& CueTag)
{
	if (CueTag.IsValid() && GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayCueParameters CueParams;
		if (AActor* AvatarActor = GetAvatarActorFromActorInfo())
		{
			CueParams.Location = AvatarActor->GetActorLocation();
		}
		GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(CueTag, CueParams);
	}
}

void UKRGA_CoreDriveBurst_Sword::ExecuteCueAtLocation(const FGameplayTag& CueTag, const FVector& Location)
{
	if (CueTag.IsValid() && GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = Location;
		GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(CueTag, CueParams);
	}
}

void UKRGA_CoreDriveBurst_Sword::PlayMontageWithCallback(UAnimMontage* Montage, void(UKRGA_CoreDriveBurst_Sword::*Callback)())
{
	if (!Montage) return;

	if (MontageTask)
	{
		MontageTask->EndTask();
		MontageTask = nullptr;
	}

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Montage, 1.0f, NAME_None, false);
	if (MontageTask)
	{
		if (Callback == &ThisClass::OnStartupMontageCompleted)
		{
			MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnStartupMontageCompleted);
			MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnStartupMontageCompleted);
		}
		else if (Callback == &ThisClass::OnAttackMontageCompleted)
		{
			MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnAttackMontageCompleted);
			MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnAttackMontageCompleted);
		}
		MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		MontageTask->ReadyForActivation();
	}
}

void UKRGA_CoreDriveBurst_Sword::SetIgnorePawnCollision(bool bIgnore)
{
	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UCapsuleComponent* Capsule = Character->GetCapsuleComponent())
		{
			if (bIgnore)
			{
				PreviousCollisionResponses.Empty();
				PreviousCollisionResponses.Add(ECC_Pawn, Capsule->GetCollisionResponseToChannel(ECC_Pawn));
				Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
			}
			else
			{
				for (const auto& Pair : PreviousCollisionResponses)
				{
					Capsule->SetCollisionResponseToChannel(Pair.Key, Pair.Value);
				}
				PreviousCollisionResponses.Empty();
			}
		}
	}
}

void UKRGA_CoreDriveBurst_Sword::OnStartupMontageCompleted()
{
	TransitionToPhase(EKRCoreDriveBurstPhase::Attack);
}

void UKRGA_CoreDriveBurst_Sword::OnAttackMontageCompleted()
{
	ExecuteCue(FinishCueTag);
	TransitionToPhase(EKRCoreDriveBurstPhase::Finished);
}

void UKRGA_CoreDriveBurst_Sword::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UKRGA_CoreDriveBurst_Sword::DisableMovement()
{
	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement())
		{
			if (bUseRootMotion)
			{
				MovementComp->StopMovementImmediately();
			}
			else
			{
				MovementComp->StopMovementImmediately();
				MovementComp->DisableMovement();
			}
		}
	}
}

void UKRGA_CoreDriveBurst_Sword::EnableMovement()
{
	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement())
		{
			if (!bUseRootMotion)
			{
				MovementComp->SetMovementMode(MOVE_Walking);
			}
		}
	}
}

void UKRGA_CoreDriveBurst_Sword::DrawDebugAttackShape()
{
	if (!bShowDebugShape || !GetWorld()) return;

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return;
	
	const FVector Origin = BurstAnchorLocation + BurstAnchorRotation.Vector() * AttackConfig.ForwardOffset;
	const float ColorLerp = static_cast<float>(CurrentHitIndex) / static_cast<float>(AttackConfig.HitCount);
	const FColor DebugColor = FColor::MakeRedToGreenColorFromScalar(1.0f - ColorLerp);

	if (AttackConfig.AttackShape == EKRAttackShapeType::Sphere)
	{
		DrawDebugSphere(GetWorld(), Origin, AttackConfig.SphereRadius, 16, DebugColor, false, DebugDrawDuration, 0, 2.0f);
	}
	else if (AttackConfig.AttackShape == EKRAttackShapeType::Box)
	{
		DrawDebugBox(GetWorld(), Origin, AttackConfig.BoxExtent, BurstAnchorRotation.Quaternion(), DebugColor, false, DebugDrawDuration, 0, 2.0f);
	}
	DrawDebugSphere(GetWorld(), AvatarActor->GetActorLocation(), 20.0f, 8, FColor::White, false, DebugDrawDuration, 0, 1.0f);
}

void UKRGA_CoreDriveBurst_Sword::CalculateStarPoints()
{
	CachedWorldPoints.Empty();
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return;

	if (StarDashData)
	{
		const float OuterRadius = StarDashData->Pattern.OuterRadius;
		const FVector Forward = BurstAnchorRotation.Vector();
		const FVector Right = FRotationMatrix(BurstAnchorRotation).GetScaledAxis(EAxis::Y);
		
		// 중심점: 앵커 위치 + 전방 거리
		const FVector Center = BurstAnchorLocation + (Forward * AttackConfig.StarCenterDistance);
		
		// 5포인트 별 그리기 순서 (Top 시작 -> 오른쪽 아래 -> 왼쪽 위 -> 오른쪽 위 -> 왼쪽 아래 -> Top)
		// 언리얼 좌표계: X Forward, Y Right. 
		// 0도 = X축(Forward). 
		// Top(0번)을 Forward 방향으로 잡기 위해 각도 조정
		
		const int32 PointCount = 5;
		// 0, 2, 4, 1, 3, 0 순서로 연결하면 별이 그려짐
		TArray<int32> DrawOrder = {0, 2, 4, 1, 3, 0}; 

		TArray<FVector> Vertices;
		for (int32 i = 0; i < PointCount; ++i)
		{
			// 0번 포인트가 정면(0도)이 되도록 설정. 시계방향(-72도씩)
			// i=0: 0도 (Forward)
			// i=1: -72도 (Right-Back)
			// ...
			float AngleDeg = -(i * 360.0f / PointCount); 
			float AngleRad = FMath::DegreesToRadians(AngleDeg);

			FVector Offset = (Forward * FMath::Cos(AngleRad) * OuterRadius) + (Right * FMath::Sin(AngleRad) * OuterRadius);
			Vertices.Add(Center + Offset);
		}

		for (int32 Index : DrawOrder)
		{
			if (Vertices.IsValidIndex(Index))
			{
				CachedWorldPoints.Add(Vertices[Index]);
			}
		}
	}
	else
	{
		// Fallback to AttackConfig manually set points
		const FVector Forward = AvatarActor->GetActorForwardVector();
		const FVector Center = AvatarActor->GetActorLocation() + (Forward * AttackConfig.StarCenterDistance);
		const FRotator Rotation = AvatarActor->GetActorRotation();

		for (const FVector& LocalPoint : AttackConfig.StarPatternPoints)
		{
			FVector WorldPoint = Center + Rotation.RotateVector(LocalPoint);
			CachedWorldPoints.Add(WorldPoint);
		}
	}
}