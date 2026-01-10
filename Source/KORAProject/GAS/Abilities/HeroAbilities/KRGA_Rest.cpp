#include "GAS/Abilities/HeroAbilities/KRGA_Rest.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Interaction/KRCheckpointActor.h"
#include "Spawner/KRWorldSpawner.h"
#include "Player/KRPlayerController.h"
#include "GameplayTag/KRAbilityTag.h"
#include "EngineUtils.h"

UKRGA_Rest::UKRGA_Rest(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FGameplayTagContainer Tags;
	Tags.AddTag(KRTAG_ABILITY_REST);
	SetAssetTags(Tags);

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UKRGA_Rest::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 체크포인트 저장 및 Save UI 표시
	SaveCheckpointAndShowUI();

	// 앉기 시작 몽타주 재생
	if (SitDownStartMontage)
	{
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			FName("SitDownStart"),
			SitDownStartMontage,
			1.0f,
			NAME_None,
			true
		);

		if (MontageTask)
		{
			MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnSitDownStartCompleted);
			MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnSitDownStartInterrupted);
			MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnSitDownStartInterrupted);
			MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnSitDownStartCompleted);
			MontageTask->ReadyForActivation();
		}
	}
	else
	{
		// 앉기 시작 몽타주가 없으면 바로 루프로 진행
		OnSitDownStartCompleted();
	}
}

void UKRGA_Rest::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(RestTimerHandle);
	}

	// 체크포인트 액터에 휴식 완료 알림
	if (AKRCheckpointActor* Checkpoint = FindCurrentCheckpoint())
	{
		Checkpoint->OnRestComplete();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_Rest::OnSitDownStartCompleted()
{
	// 모든 적 재스폰
	RespawnAllEnemies();

	// 앉기 루프 몽타주 재생
	PlaySitLoopMontage();
}

void UKRGA_Rest::OnSitDownStartInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UKRGA_Rest::PlaySitLoopMontage()
{
	// 이전 SitDownStart 태스크의 델리게이트 해제
	if (MontageTask)
	{
		MontageTask->OnCompleted.RemoveDynamic(this, &ThisClass::OnSitDownStartCompleted);
		MontageTask->OnInterrupted.RemoveDynamic(this, &ThisClass::OnSitDownStartInterrupted);
		MontageTask->OnCancelled.RemoveDynamic(this, &ThisClass::OnSitDownStartInterrupted);
		MontageTask->OnBlendOut.RemoveDynamic(this, &ThisClass::OnSitDownStartCompleted);
		MontageTask = nullptr;
	}

	if (SitLoopMontage)
	{
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			FName("SitLoop"),
			SitLoopMontage,
			1.0f,
			NAME_None,
			true
		);

		if (MontageTask)
		{
			MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnSitLoopCompleted);
			MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnSitLoopInterrupted);
			MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnSitLoopInterrupted);
			MontageTask->ReadyForActivation();
		}
	}

	// 휴식 타이머 시작 (루프 재생 중 대기)
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			RestTimerHandle,
			this,
			&ThisClass::OnRestTimerExpired,
			RestDuration,
			false
		);
	}
}

void UKRGA_Rest::OnSitLoopCompleted()
{
	// 루프가 끝나도 타이머가 만료될 때까지 대기
	// 타이머가 이미 만료되었다면 아무 동작 안 함
}

void UKRGA_Rest::OnSitLoopInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UKRGA_Rest::OnRestTimerExpired()
{
	// 이전 SitLoop 태스크의 델리게이트 해제 (조기 종료 방지)
	if (MontageTask)
	{
		MontageTask->OnCompleted.RemoveDynamic(this, &ThisClass::OnSitLoopCompleted);
		MontageTask->OnInterrupted.RemoveDynamic(this, &ThisClass::OnSitLoopInterrupted);
		MontageTask->OnCancelled.RemoveDynamic(this, &ThisClass::OnSitLoopInterrupted);
		MontageTask = nullptr;
	}

	// 일어나기 몽타주 재생
	if (StandUpMontage)
	{
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			FName("StandUp"),
			StandUpMontage,
			1.0f,
			NAME_None,
			true
		);

		if (MontageTask)
		{
			MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnStandUpCompleted);
			MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnStandUpInterrupted);
			MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnStandUpInterrupted);
			MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnStandUpCompleted);
			MontageTask->ReadyForActivation();
		}
	}
	else
	{
		// 일어나기 몽타주가 없으면 바로 종료
		OnStandUpCompleted();
	}
}

void UKRGA_Rest::OnStandUpCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UKRGA_Rest::OnStandUpInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UKRGA_Rest::SaveCheckpointAndShowUI()
{
	if (AKRCheckpointActor* Checkpoint = FindCurrentCheckpoint())
	{
		Checkpoint->SaveCheckpoint();
	}
}

void UKRGA_Rest::RespawnAllEnemies()
{
	UWorld* World = GetWorld();
	if (!World) return;

	// 모든 KRWorldSpawner 찾아서 RespawnAll 호출
	for (TActorIterator<AKRWorldSpawner> It(World); It; ++It)
	{
		AKRWorldSpawner* Spawner = *It;
		if (Spawner)
		{
			Spawner->RespawnAll();
			UE_LOG(LogTemp, Log, TEXT("[GA_Rest] Respawning enemies from spawner: %s"), *Spawner->GetName());
		}
	}
}

AKRCheckpointActor* UKRGA_Rest::FindCurrentCheckpoint() const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return nullptr;

	APawn* Pawn = Cast<APawn>(AvatarActor);
	if (!Pawn) return nullptr;

	AKRPlayerController* PC = Cast<AKRPlayerController>(Pawn->GetController());
	if (!PC) return nullptr;

	// PlayerController에서 현재 상호작용 중인 액터 가져오기
	AActor* InteractableActor = PC->GetCurrentInteractableActor();
	return Cast<AKRCheckpointActor>(InteractableActor);
}