#include "Interaction/KRCheckpointActor.h"
#include "Game/KRGameInstance.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GAS/Abilities/KRGameplayAbility.h"

AKRCheckpointActor::AKRCheckpointActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// 기본 Niagara 컴포넌트 (상시 재생용)
	IdleEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("IdleEffect"));
	IdleEffectComponent->SetupAttachment(RootComponent);
	IdleEffectComponent->bAutoActivate = true;
}

void AKRCheckpointActor::BeginPlay()
{
	Super::BeginPlay();
}

void AKRCheckpointActor::OnAbilityActivated(UGameplayAbility* Ability)
{
	Super::OnAbilityActivated(Ability);

	// 상호작용 어빌리티가 활성화되면 체크포인트 저장 및 UI 표시
	if (bIsInteract)
	{
		// 활성화 이펙트 재생
		if (ActivationEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				this,
				ActivationEffect,
				GetActorLocation(),
				GetActorRotation()
			);
		}

		// 활성화 사운드 재생
		if (ActivationSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ActivationSound, GetActorLocation());
		}

		bIsActivated = true;
	}
}

void AKRCheckpointActor::SaveCheckpoint()
{
	if (UKRGameInstance* GI = Cast<UKRGameInstance>(GetGameInstance()))
	{
		FTransform SpawnTransform = GetSpawnTransform();
		GI->SaveCheckpoint(SpawnTransform, CheckpointTag);
	}

	// Save UI 브로드캐스트
	BroadcastSaveUI();
}

FTransform AKRCheckpointActor::GetSpawnTransform() const
{
	FVector SpawnLocation = GetActorLocation() + GetActorRotation().RotateVector(SpawnOffset);
	FRotator SpawnRotation = GetActorRotation();
	SpawnRotation.Yaw += SpawnYawOffset;

	return FTransform(SpawnRotation, SpawnLocation, FVector::OneVector);
}

void AKRCheckpointActor::OnRestComplete()
{
	// 휴식 완료 시 추가 처리 (필요시 확장)
	bIsInteract = false;
}

void AKRCheckpointActor::BroadcastSaveUI()
{
	FKRUIMessage_Info SaveMessage;
	SaveMessage.Context = EInfoContext::SaveGame;
	SaveMessage.StringTableKey = TEXT("Info_SaveGame");

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(FKRUIMessageTags::SaveDeathLevelInfo(), SaveMessage);
}
