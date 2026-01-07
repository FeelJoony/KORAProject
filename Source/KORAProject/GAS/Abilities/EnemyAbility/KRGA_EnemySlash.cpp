#include "GAS/Abilities/EnemyAbility/KRGA_EnemySlash.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Characters/KREnemyCharacter.h"
#include "GameplayTag/KREnemyTag.h"
#include "GameplayTag/KREventTag.h"
#include "GameplayTag/KRSetByCallerTag.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "Engine/OverlapResult.h"

UKRGA_EnemySlash::UKRGA_EnemySlash(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 기본 히트 체크 이벤트 태그 설정
	HitCheckEventTag = KRTAG_EVENT_ENEMY_SLASH_HITCHECK;
}

void UKRGA_EnemySlash::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
										const FGameplayAbilityActorInfo* ActorInfo,
										const FGameplayAbilityActivationInfo ActivationInfo,
										const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// 히트 액터 초기화
	HitActors.Empty();

	// 몽타주 재생
	if (SlashMontage)
	{
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			SlashMontage,
			1.f,
			NAME_None
		);

		if (MontageTask)
		{
			MontageTask->OnCompleted.AddDynamic(this, &UKRGA_EnemySlash::OnMontageEnded);
			MontageTask->OnCancelled.AddDynamic(this, &UKRGA_EnemySlash::OnMontageEnded);
			MontageTask->OnInterrupted.AddDynamic(this, &UKRGA_EnemySlash::OnMontageEnded);
			MontageTask->OnBlendOut.AddDynamic(this, &UKRGA_EnemySlash::OnMontageEnded);
			MontageTask->ReadyForActivation();
		}
	}

	// GameplayEvent 리스너 설정
	SetupHitCheckEventListener();
}

void UKRGA_EnemySlash::EndAbility(const FGameplayAbilitySpecHandle Handle,
									const FGameplayAbilityActorInfo* ActorInfo,
									const FGameplayAbilityActivationInfo ActivationInfo,
									bool bReplicateEndAbility, bool bWasCancelled)
{
	HitActors.Empty();

	AKREnemyCharacter* Enemy = Cast<AKREnemyCharacter>(GetAvatarActorFromActorInfo());
	if (Enemy)
	{
		UKRAbilitySystemComponent* EnemyASC = Enemy->GetEnemyAbilitySystemCompoent();
		if (EnemyASC && EnemyASC->HasMatchingGameplayTag(KRTAG_ENEMY_ACTION_SLASH))
		{
			EnemyASC->RemoveLooseGameplayTag(KRTAG_ENEMY_ACTION_SLASH);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_EnemySlash::OnMontageEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UKRGA_EnemySlash::SetupHitCheckEventListener()
{
	if (!HitCheckEventTag.IsValid())
	{
		return;
	}

	HitCheckEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		HitCheckEventTag,
		nullptr,
		false,  // OnlyTriggerOnce = false (여러 번 수신 가능)
		true    // OnlyMatchExact = true
	);

	if (HitCheckEventTask)
	{
		HitCheckEventTask->EventReceived.AddDynamic(this, &UKRGA_EnemySlash::OnHitCheckEventReceived);
		HitCheckEventTask->ReadyForActivation();
	}
}

void UKRGA_EnemySlash::OnHitCheckEventReceived(FGameplayEventData Payload)
{
	PerformHitCheck();
}

void UKRGA_EnemySlash::PerformHitCheck()
{
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

	// 전방 박스 오버랩으로 히트 체크
	const FVector Origin = AvatarActor->GetActorLocation();
	const FVector Forward = AvatarActor->GetActorForwardVector();
	const FVector BoxCenter = Origin + Forward * 100.0f;  // 전방 100 유닛
	const FVector BoxExtent = FVector(100.0f, 100.0f, 100.0f);

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);

	bool bHit = World->OverlapMultiByChannel(
		OverlapResults,
		BoxCenter,
		AvatarActor->GetActorQuat(),
		ECC_Pawn,
		FCollisionShape::MakeBox(BoxExtent),
		QueryParams
	);

	if (bHit)
	{
		for (const FOverlapResult& Overlap : OverlapResults)
		{
			AActor* HitActor = Overlap.GetActor();
			if (!HitActor || HitActors.Contains(HitActor))
			{
				continue;
			}

			// 자기 자신 제외
			if (HitActor == AvatarActor)
			{
				continue;
			}

			// ASC 보유 대상에게만 데미지
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
			if (!TargetASC)
			{
				continue;
			}

			// 데미지 적용
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

			HitActors.Add(HitActor);
		}
	}
}