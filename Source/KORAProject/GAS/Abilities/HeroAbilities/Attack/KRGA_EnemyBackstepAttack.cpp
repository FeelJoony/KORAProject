#include "GAS/Abilities/HeroAbilities/Attack/KRGA_EnemyBackstepAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Components/StateTreeAIComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"
#include "GameplayTag/KREventTag.h"
#include "GameplayTag/KRSetByCallerTag.h"
#include "Kismet/GameplayStatics.h"
#include "GAS/Abilities/Tasks/KRAbilityTask_WaitTick.h"
#include "DrawDebugHelpers.h"
#include "Characters/KRHeroCharacter.h"

UKRGA_EnemyBackstepAttack::UKRGA_EnemyBackstepAttack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UKRGA_EnemyBackstepAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	HitActorsThisSwing.Empty();
	bIsHitCheckActive = false;

	if (ActorInfo)
	{
		for (TSubclassOf<UGameplayEffect> EffectClass : GameplayEffectClassesWhileAnimating)
		{
			if (EffectClass)
			{
				const UGameplayEffect* Effect = EffectClass->GetDefaultObject<UGameplayEffect>();
				UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
				if (ASC && Effect)
				{
					FActiveGameplayEffectHandle EffectHandle = ASC->ApplyGameplayEffectToSelf(
						Effect, 1.f, MakeEffectContext(Handle, ActorInfo));
					if (EffectHandle.IsValid())
					{
						AppliedEffects.Add(EffectHandle);
					}
				}
			}
		}
	}

	SetupHitCheckEventListeners();

	if (ActorInfo->OwnerActor.IsValid())
	{
		if (AActor* OwnerActor = ActorInfo->OwnerActor.Get())
		{
			if (UStateTreeAIComponent* StateTreeAIComp = OwnerActor->GetComponentByClass<UStateTreeAIComponent>())
			{
				if (AbilityTags.Num() > 0)
				{
					StateTreeAIComp->SendStateTreeEvent(AbilityTags.GetByIndex(0));
				}
			}
		}
	}

	StartBackstepPhase();
}

void UKRGA_EnemyBackstepAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (BackstepTickTask)
	{
		BackstepTickTask->EndTask();
		BackstepTickTask = nullptr;
	}

	HitActorsThisSwing.Empty();
	bIsHitCheckActive = false;
	CurrentPhase = EAttackPhase::None;
	CachedPlayerActor.Reset();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_EnemyBackstepAttack::StartBackstepPhase()
{
	CurrentPhase = EAttackPhase::Backstep;

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(AvatarActor->GetWorld(), 0);
	if (!PlayerCharacter)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	CachedPlayerActor = PlayerCharacter;

	FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	FVector EnemyLocation = AvatarActor->GetActorLocation();
	FVector DirectionFromPlayer = (EnemyLocation - PlayerLocation).GetSafeNormal2D();
	BackstepTargetLocation = PlayerLocation + (DirectionFromPlayer * BackstepDistance);
	BackstepTargetLocation.Z = EnemyLocation.Z;

	BackstepTargetRotation = AvatarActor->GetActorRotation();
	if (bFacePlayerOnBackstep)
	{
		FVector DirectionToPlayer = (PlayerLocation - BackstepTargetLocation).GetSafeNormal2D();
		BackstepTargetRotation = DirectionToPlayer.Rotation();
	}
	AvatarActor->SetActorRotation(BackstepTargetRotation);

	if (BackstepMontage)
	{
		BackstepMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, FName("BackstepMontage"), BackstepMontage, 1.0f, NAME_None, false);

		if (BackstepMontageTask)
		{
			BackstepMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnBackstepMontageCompleted);
			BackstepMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnBackstepMontageCompleted);
			BackstepMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnBackstepMontageInterrupted);
			BackstepMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnBackstepMontageInterrupted);
			BackstepMontageTask->ReadyForActivation();
		}
	}

	BackstepTickTask = UKRAbilityTask_WaitTick::WaitTick(this, FName("BackstepTick"));
	if (BackstepTickTask)
	{
		BackstepTickTask->OnTick.AddDynamic(this, &ThisClass::OnBackstepTick);
		BackstepTickTask->ReadyForActivation();
	}
	else if (!BackstepMontage)
	{
		StartAttackPhase();
	}
}

void UKRGA_EnemyBackstepAttack::OnBackstepTick(float DeltaTime)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	FVector CurrentLocation = AvatarActor->GetActorLocation();
	FVector ToTarget = BackstepTargetLocation - CurrentLocation;
	ToTarget.Z = 0.f;

	float DistanceToTarget = ToTarget.Size();
	float MoveDistance = BackstepSpeed * DeltaTime;

	if (DistanceToTarget <= MoveDistance || DistanceToTarget < 10.f)
	{
		FVector FinalLocation = BackstepTargetLocation;
		FinalLocation.Z = CurrentLocation.Z;
		AvatarActor->SetActorLocation(FinalLocation);

		if (BackstepTickTask)
		{
			BackstepTickTask->EndTask();
			BackstepTickTask = nullptr;
		}

		if (!BackstepMontage)
		{
			StartAttackPhase();
		}
		return;
	}

	FVector MoveDirection = ToTarget.GetSafeNormal();
	FVector NewLocation = CurrentLocation + (MoveDirection * MoveDistance);
	AvatarActor->SetActorLocation(NewLocation);
}

void UKRGA_EnemyBackstepAttack::StartAttackPhase()
{
	CurrentPhase = EAttackPhase::Attack;

	if (!MontageToPlay)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	AttackMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, FName("AttackMontage"), MontageToPlay, AttackMontagePlayRate, NAME_None, false);

	if (AttackMontageTask)
	{
		AttackMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnAttackMontageCompleted);
		AttackMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnAttackMontageCompleted);
		AttackMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnAttackMontageInterrupted);
		AttackMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnAttackMontageInterrupted);
		AttackMontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

void UKRGA_EnemyBackstepAttack::StartRecoveryPhase()
{
	CurrentPhase = EAttackPhase::Recovery;

	if (!BackstepMontage)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	RecoveryMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, FName("RecoveryMontage"), BackstepMontage, RecoveryMontagePlayRate, NAME_None, false);

	if (RecoveryMontageTask)
	{
		RecoveryMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnRecoveryMontageCompleted);
		RecoveryMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnRecoveryMontageCompleted);
		RecoveryMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnRecoveryMontageInterrupted);
		RecoveryMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnRecoveryMontageInterrupted);
		RecoveryMontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UKRGA_EnemyBackstepAttack::OnBackstepMontageCompleted()
{
	if (BackstepTickTask)
	{
		BackstepTickTask->EndTask();
		BackstepTickTask = nullptr;
	}
	StartAttackPhase();
}

void UKRGA_EnemyBackstepAttack::OnBackstepMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UKRGA_EnemyBackstepAttack::OnAttackMontageCompleted()
{
	EndHitCheck();
	StartRecoveryPhase();
}

void UKRGA_EnemyBackstepAttack::OnAttackMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UKRGA_EnemyBackstepAttack::OnRecoveryMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UKRGA_EnemyBackstepAttack::OnRecoveryMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UKRGA_EnemyBackstepAttack::PerformHitCheck()
{
	if (!bIsHitCheckActive) return;

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return;
	}

	UWorld* World = AvatarActor->GetWorld();
	if (!World)
	{
		return;
	}

	const FVector Origin = AvatarActor->GetActorLocation();
	const FVector Forward = AvatarActor->GetActorForwardVector();
	const FVector SphereCenter = Origin + Forward * HitCheckForwardOffset;

#if ENABLE_DRAW_DEBUG
	DrawDebugSphere(World, SphereCenter, HitCheckSphereRadius, 16, FColor::Red, false, 2.0f);
	if (CachedPlayerActor.IsValid())
	{
		DrawDebugSphere(World, CachedPlayerActor->GetActorLocation(), 50.0f, 12, FColor::Green, false, 2.0f);
	}
#endif

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);

	bool bHit = World->OverlapMultiByChannel(
		OverlapResults,
		SphereCenter,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(HitCheckSphereRadius),
		QueryParams
	);

	if (bHit)
	{
		for (const FOverlapResult& Overlap : OverlapResults)
		{
			AActor* HitActor = Overlap.GetActor();
			if (!HitActor || HitActorsThisSwing.Contains(HitActor))
			{
				continue;
			}

			if (HitActor == AvatarActor)
			{
				continue;
			}

			if (!HitActor->IsA<AKRHeroCharacter>())
			{
				continue;
			}
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
			if (!TargetASC)
			{
				UE_LOG(LogTemp, Warning, TEXT("[BackstepAttack] TargetASC is null!"));
				continue;
			}

			if (DamageEffectClass)
			{
				UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
				if (SourceASC)
				{
					FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());
					if (SpecHandle.IsValid())
					{
						SpecHandle.Data->SetSetByCallerMagnitude(KRTAG_SETBYCALLER_BASEDAMAGE, BaseDamage);
						SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
					}
				}
			}
			HitActorsThisSwing.Add(HitActor);
		}
	}
}
