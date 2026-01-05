
#include "KRAerialComboTestActor.h"
/*
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "SubSystem/KREffectSubsystem.h"

AKRAerialComboTestActor::AKRAerialComboTestActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// 캡슐 컴포넌트 생성 (타겟 감지용)
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->InitCapsuleSize(42.0f, 96.0f);
	SetRootComponent(CapsuleComponent);

	// 메시 컴포넌트 생성 (시각적 표현용, 선택적)
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -96.0f));

	// 태그 추가 (적으로 인식되도록)
	Tags.Add(FName("Enemy"));
}

void AKRAerialComboTestActor::BeginPlay()
{
	Super::BeginPlay();

	// 시작 위치 저장
	TargetStartLocation = GetActorLocation();

	// 자동 시작
	if (bAutoStartOnBeginPlay)
	{
		GetWorldTimerManager().SetTimer(
			AutoStartTimerHandle,
			this,
			&AKRAerialComboTestActor::StartAerialCombo,
			AutoStartDelay,
			false
		);
	}
}

void AKRAerialComboTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 디버그 드로우
	if (bDrawDebugInfo || bDrawDebugTarget)
	{
		DrawDebug();
	}

	// 슬램 중 플레이어 위치 보간
	if (bIsSlamming)
	{
		ACharacter* Character = GetPlayerCharacter();
		if (Character && Character->GetCharacterMovement())
		{
			float ElapsedTime = GetWorld()->GetTimeSeconds() - SlamStartTime;
			float SlamDuration = 0.5f;  // 슬램 최대 시간
			float Alpha = FMath::Clamp(ElapsedTime / SlamDuration, 0.0f, 1.0f);

			// 이징 함수 적용 (EaseIn - 가속)
			float EasedAlpha = FMath::Pow(Alpha, 2.0f);

			// 타겟(이 Actor) 방향으로 슬램
			FVector TargetLocation = GetActorLocation();
			TargetLocation.Z = SkillStartLocation.Z;  // 바닥 높이로

			FVector NewLocation = FMath::Lerp(SlamStartLocation, TargetLocation, EasedAlpha);
			Character->SetActorLocation(NewLocation);

			// 착지 체크 (바닥 근처)
			if (!Character->GetCharacterMovement()->IsFalling() || Alpha >= 1.0f)
			{
				bIsSlamming = false;
				ExecuteLandingPhase();
			}
		}
	}
}

// ─────────────────────────────────────────────────────
// 메인 함수
// ─────────────────────────────────────────────────────

void AKRAerialComboTestActor::StartAerialCombo()
{
	ACharacter* Character = GetPlayerCharacter();
	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("AerialComboTestActor: Player character not found!"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("AerialComboTestActor: Starting Aerial Combo!"));

	// 시작 위치 저장
	SkillStartLocation = Character->GetActorLocation();

	// 원래 중력 스케일 저장
	if (Character->GetCharacterMovement())
	{
		OriginalGravityScale = Character->GetCharacterMovement()->GravityScale;
	}

	// 플레이어가 타겟(이 Actor)을 바라보도록
	FVector DirectionToTarget = (GetActorLocation() - Character->GetActorLocation()).GetSafeNormal2D();
	if (!DirectionToTarget.IsNearlyZero())
	{
		FRotator LookRotation = DirectionToTarget.Rotation();
		Character->SetActorRotation(LookRotation);
	}

	// 런칭 페이즈 시작
	ExecuteLaunchPhase();
}

void AKRAerialComboTestActor::StopAerialCombo()
{
	// 타이머 클리어
	GetWorldTimerManager().ClearTimer(PhaseTimerHandle);
	GetWorldTimerManager().ClearTimer(LandingCheckTimerHandle);
	GetWorldTimerManager().ClearTimer(AutoStartTimerHandle);

	// 중력 복구
	RestorePlayerGravityScale();

	// 이펙트 정리
	StopAfterImageEffect();
	if (ActiveSlamTrailEffect)
	{
		ActiveSlamTrailEffect->Deactivate();
		ActiveSlamTrailEffect = nullptr;
	}

	// 상태 초기화
	CurrentPhase = ETestAerialComboPhase::None;
	bIsSlamming = false;

	// 몽타주 중지
	StopCurrentMontage();

	UE_LOG(LogTemp, Warning, TEXT("AerialComboTestActor: Aerial Combo stopped."));
}

// ─────────────────────────────────────────────────────
// 데이터 에셋 헬퍼
// ─────────────────────────────────────────────────────

float AKRAerialComboTestActor::GetEnemyLaunchVelocity() const
{
	return (bUseDataAsset && AerialComboData) ? AerialComboData->Launch.EnemyLaunchVelocity : EnemyLaunchVelocity;
}

float AKRAerialComboTestActor::GetPlayerJumpVelocity() const
{
	return (bUseDataAsset && AerialComboData) ? AerialComboData->Launch.PlayerJumpVelocity : PlayerJumpVelocity;
}

float AKRAerialComboTestActor::GetAirborneHangTime() const
{
	return (bUseDataAsset && AerialComboData) ? AerialComboData->Launch.AirborneHangTime : AirborneHangTime;
}

float AKRAerialComboTestActor::GetSlamVelocity() const
{
	return (bUseDataAsset && AerialComboData) ? AerialComboData->Slam.SlamVelocity : SlamVelocity;
}

float AKRAerialComboTestActor::GetImpactRadius() const
{
	return (bUseDataAsset && AerialComboData) ? AerialComboData->Slam.ImpactRadius : ImpactRadius;
}

float AKRAerialComboTestActor::GetSlamHitStopDuration() const
{
	return (bUseDataAsset && AerialComboData) ? AerialComboData->Slam.HitStopDuration : SlamHitStopDuration;
}

float AKRAerialComboTestActor::GetAirborneGravityScale() const
{
	return (bUseDataAsset && AerialComboData) ? AerialComboData->Physics.AirborneGravityScale : AirborneGravityScale;
}

float AKRAerialComboTestActor::GetSlamGravityScale() const
{
	return (bUseDataAsset && AerialComboData) ? AerialComboData->Physics.SlamGravityScale : SlamGravityScale;
}

float AKRAerialComboTestActor::GetMontagePlayRate() const
{
	return (bUseDataAsset && AerialComboData) ? AerialComboData->Montage.MontagePlayRate : MontagePlayRate;
}

UAnimMontage* AKRAerialComboTestActor::GetLaunchMontage() const
{
	return (bUseDataAsset && AerialComboData && AerialComboData->Montage.LaunchMontage) ? AerialComboData->Montage.LaunchMontage : LaunchMontage;
}

UAnimMontage* AKRAerialComboTestActor::GetRisingMontage() const
{
	return (bUseDataAsset && AerialComboData && AerialComboData->Montage.RisingMontage) ? AerialComboData->Montage.RisingMontage : RisingMontage;
}

UAnimMontage* AKRAerialComboTestActor::GetAirborneMontage() const
{
	return (bUseDataAsset && AerialComboData && AerialComboData->Montage.AirborneMontage) ? AerialComboData->Montage.AirborneMontage : AirborneMontage;
}

UAnimMontage* AKRAerialComboTestActor::GetSlamMontage() const
{
	return (bUseDataAsset && AerialComboData && AerialComboData->Montage.SlamMontage) ? AerialComboData->Montage.SlamMontage : SlamMontage;
}

UAnimMontage* AKRAerialComboTestActor::GetLandingMontage() const
{
	return (bUseDataAsset && AerialComboData && AerialComboData->Montage.LandingMontage) ? AerialComboData->Montage.LandingMontage : LandingMontage;
}

FGameplayTag AKRAerialComboTestActor::GetLaunchWindEffectTag() const
{
	if (bUseDataAsset && AerialComboData)
	{
		return AerialComboData->Effects.LaunchWindEffectTag;
	}
	return LaunchWindEffectTag;
}

FGameplayTag AKRAerialComboTestActor::GetAfterImageEffectTag() const
{
	if (bUseDataAsset && AerialComboData)
	{
		return AerialComboData->Effects.AfterImageEffectTag;
	}
	return AfterImageEffectTag;
}

FGameplayTag AKRAerialComboTestActor::GetSlamTrailEffectTag() const
{
	if (bUseDataAsset && AerialComboData)
	{
		return AerialComboData->Effects.SlamTrailEffectTag;
	}
	return SlamTrailEffectTag;
}

FGameplayTag AKRAerialComboTestActor::GetImpactEffectTag() const
{
	if (bUseDataAsset && AerialComboData)
	{
		return AerialComboData->Effects.ImpactEffectTag;
	}
	return ImpactEffectTag;
}

FGameplayTag AKRAerialComboTestActor::GetGroundCrackEffectTag() const
{
	if (bUseDataAsset && AerialComboData)
	{
		return AerialComboData->Effects.GroundCrackEffectTag;
	}
	return GroundCrackEffectTag;
}

FLinearColor AKRAerialComboTestActor::GetEffectColor() const
{
	return (bUseDataAsset && AerialComboData) ? AerialComboData->Effects.EffectColor : EffectColor;
}

// ─────────────────────────────────────────────────────
// 페이즈 실행
// ─────────────────────────────────────────────────────

void AKRAerialComboTestActor::ExecuteLaunchPhase()
{
	CurrentPhase = ETestAerialComboPhase::Launch;
	UE_LOG(LogTemp, Log, TEXT("AerialComboTestActor: Launch Phase"));

	// 런칭 이펙트 스폰
	SpawnLaunchEffect();

	// 런칭 몽타주 재생
	UAnimMontage* CurrentLaunchMontage = GetLaunchMontage();
	if (CurrentLaunchMontage)
	{
		PlayMontage(CurrentLaunchMontage, GetMontagePlayRate());
	}

	// 타겟(이 Actor)을 위로 띄우는 것은 시뮬레이션만 (실제 움직임 없음)
	// 실제 게임에서는 적 캐릭터가 띄워지겠지만 테스트에서는 생략

	// 딜레이 후 상승 페이즈
	float LaunchDuration = CurrentLaunchMontage ? CurrentLaunchMontage->GetPlayLength() / GetMontagePlayRate() : 0.3f;
	GetWorldTimerManager().SetTimer(
		PhaseTimerHandle,
		this,
		&AKRAerialComboTestActor::ExecuteRisingPhase,
		LaunchDuration,
		false
	);
}

void AKRAerialComboTestActor::ExecuteRisingPhase()
{
	CurrentPhase = ETestAerialComboPhase::Rising;
	UE_LOG(LogTemp, Log, TEXT("AerialComboTestActor: Rising Phase"));

	// 플레이어 점프
	LaunchPlayer();

	// 잔상 이펙트 시작
	StartAfterImageEffect();

	// 상승 몽타주 재생
	UAnimMontage* CurrentRisingMontage = GetRisingMontage();
	if (CurrentRisingMontage)
	{
		PlayMontage(CurrentRisingMontage, GetMontagePlayRate());
	}

	// 딜레이 후 공중 페이즈
	float RisingDuration = 0.3f;
	GetWorldTimerManager().SetTimer(
		PhaseTimerHandle,
		this,
		&AKRAerialComboTestActor::ExecuteAirbornePhase,
		RisingDuration,
		false
	);
}

void AKRAerialComboTestActor::ExecuteAirbornePhase()
{
	CurrentPhase = ETestAerialComboPhase::Airborne;
	UE_LOG(LogTemp, Log, TEXT("AerialComboTestActor: Airborne Phase"));

	// 공중 몽타주 재생
	UAnimMontage* CurrentAirborneMontage = GetAirborneMontage();
	if (CurrentAirborneMontage)
	{
		PlayMontage(CurrentAirborneMontage, GetMontagePlayRate());
	}

	// 공중 체류 시간 후 슬램 페이즈
	GetWorldTimerManager().SetTimer(
		PhaseTimerHandle,
		this,
		&AKRAerialComboTestActor::ExecuteSlamPhase,
		GetAirborneHangTime(),
		false
	);
}

void AKRAerialComboTestActor::ExecuteSlamPhase()
{
	CurrentPhase = ETestAerialComboPhase::Slam;
	UE_LOG(LogTemp, Log, TEXT("AerialComboTestActor: Slam Phase"));

	ACharacter* Character = GetPlayerCharacter();
	if (!Character)
	{
		CompleteAerialCombo();
		return;
	}

	// 슬램 중력 스케일
	SetPlayerGravityScale(GetSlamGravityScale());

	// 슬램 궤적 이펙트
	SpawnSlamTrailEffect();

	// 슬램 몽타주 재생
	UAnimMontage* CurrentSlamMontage = GetSlamMontage();
	if (CurrentSlamMontage)
	{
		PlayMontage(CurrentSlamMontage, GetMontagePlayRate());
	}

	// 슬램 시작 정보 저장
	SlamStartLocation = Character->GetActorLocation();
	SlamStartTime = GetWorld()->GetTimeSeconds();
	bIsSlamming = true;

	// 슬램 속도 부여
	if (Character->GetCharacterMovement())
	{
		FVector SlamDirection = (GetActorLocation() - Character->GetActorLocation()).GetSafeNormal();
		SlamDirection.Z = -1.0f;  // 아래 방향 강조
		SlamDirection.Normalize();
		Character->GetCharacterMovement()->Velocity = SlamDirection * GetSlamVelocity();
	}
}

void AKRAerialComboTestActor::ExecuteLandingPhase()
{
	CurrentPhase = ETestAerialComboPhase::Landing;
	UE_LOG(LogTemp, Log, TEXT("AerialComboTestActor: Landing Phase"));

	// 중력 복구
	RestorePlayerGravityScale();

	// 히트스톱
	ApplyHitStop(GetSlamHitStopDuration());

	// 착지 충격파 이펙트
	SpawnImpactEffect();
	SpawnGroundCrackEffect();

	// 잔상 이펙트 중지
	StopAfterImageEffect();

	// 슬램 궤적 이펙트 중지
	if (ActiveSlamTrailEffect)
	{
		ActiveSlamTrailEffect->Deactivate();
		ActiveSlamTrailEffect = nullptr;
	}

	// 착지 몽타주 재생
	UAnimMontage* CurrentLandingMontage = GetLandingMontage();
	if (CurrentLandingMontage)
	{
		PlayMontage(CurrentLandingMontage, GetMontagePlayRate());
		float LandingDuration = CurrentLandingMontage->GetPlayLength() / GetMontagePlayRate();
		GetWorldTimerManager().SetTimer(
			PhaseTimerHandle,
			this,
			&AKRAerialComboTestActor::CompleteAerialCombo,
			LandingDuration,
			false
		);
	}
	else
	{
		// 딜레이 후 완료
		GetWorldTimerManager().SetTimer(
			PhaseTimerHandle,
			this,
			&AKRAerialComboTestActor::CompleteAerialCombo,
			0.5f,
			false
		);
	}
}

void AKRAerialComboTestActor::CompleteAerialCombo()
{
	CurrentPhase = ETestAerialComboPhase::Recovery;
	UE_LOG(LogTemp, Warning, TEXT("AerialComboTestActor: Aerial Combo complete!"));

	// 중력 복구 (안전하게 한 번 더)
	RestorePlayerGravityScale();

	// 상태 초기화
	CurrentPhase = ETestAerialComboPhase::None;
	bIsSlamming = false;
}

// ─────────────────────────────────────────────────────
// 캐릭터 제어
// ─────────────────────────────────────────────────────

ACharacter* AKRAerialComboTestActor::GetPlayerCharacter() const
{
	return UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

void AKRAerialComboTestActor::LaunchPlayer()
{
	ACharacter* Character = GetPlayerCharacter();
	if (!Character)
	{
		return;
	}

	UCharacterMovementComponent* Movement = Character->GetCharacterMovement();
	if (!Movement)
	{
		return;
	}

	// 플레이어 점프
	FVector LaunchVelocity = FVector(0.0f, 0.0f, GetPlayerJumpVelocity());

	// 타겟 방향으로 약간 이동
	FVector ToTarget = (GetActorLocation() - Character->GetActorLocation()).GetSafeNormal2D();
	LaunchVelocity += ToTarget * 300.0f;

	Character->LaunchCharacter(LaunchVelocity, false, true);

	// 공중에서 중력 감소
	Movement->GravityScale = GetAirborneGravityScale();

	UE_LOG(LogTemp, Log, TEXT("AerialComboTestActor: Player launched with velocity %s"), *LaunchVelocity.ToString());
}

void AKRAerialComboTestActor::SetPlayerGravityScale(float Scale)
{
	ACharacter* Character = GetPlayerCharacter();
	if (Character && Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->GravityScale = Scale;
	}
}

void AKRAerialComboTestActor::RestorePlayerGravityScale()
{
	ACharacter* Character = GetPlayerCharacter();
	if (Character && Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->GravityScale = OriginalGravityScale;
	}
}

void AKRAerialComboTestActor::CheckLanding()
{
	ACharacter* Character = GetPlayerCharacter();
	if (!Character)
	{
		return;
	}

	if (!Character->GetCharacterMovement()->IsFalling())
	{
		GetWorldTimerManager().ClearTimer(LandingCheckTimerHandle);
		ExecuteLandingPhase();
	}
}

// ─────────────────────────────────────────────────────
// 몽타주 제어
// ─────────────────────────────────────────────────────

void AKRAerialComboTestActor::PlayMontage(UAnimMontage* Montage, float PlayRate)
{
	if (!Montage)
	{
		return;
	}

	ACharacter* Character = GetPlayerCharacter();
	if (!Character)
	{
		return;
	}

	USkeletalMeshComponent* Mesh = Character->GetMesh();
	if (!Mesh)
	{
		return;
	}

	UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}

	float Duration = AnimInstance->Montage_Play(Montage, PlayRate);
	UE_LOG(LogTemp, Log, TEXT("AerialComboTestActor: Playing montage '%s' (Duration: %.3f)"),
		*Montage->GetName(), Duration);
}

void AKRAerialComboTestActor::StopCurrentMontage()
{
	ACharacter* Character = GetPlayerCharacter();
	if (!Character)
	{
		return;
	}

	USkeletalMeshComponent* Mesh = Character->GetMesh();
	if (!Mesh)
	{
		return;
	}

	UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Stop(0.2f);
	}
}

// ─────────────────────────────────────────────────────
// 이펙트 제어
// ─────────────────────────────────────────────────────

void AKRAerialComboTestActor::SpawnLaunchEffect()
{
	FGameplayTag EffectTag = GetLaunchWindEffectTag();
	if (!EffectTag.IsValid())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UKREffectSubsystem* EffectSubsystem = World->GetSubsystem<UKREffectSubsystem>();
	if (EffectSubsystem)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(GetActorLocation());
		SpawnTransform.SetRotation(FQuat(FRotator(-90.0f, 0.0f, 0.0f)));  // 위쪽 방향

		EffectSubsystem->SpawnEffectByTag(EffectTag, SpawnTransform, this);
		UE_LOG(LogTemp, Log, TEXT("AerialComboTestActor: Launch wind effect spawned by tag"));
	}
}

void AKRAerialComboTestActor::StartAfterImageEffect()
{
	FGameplayTag EffectTag = GetAfterImageEffectTag();
	if (!EffectTag.IsValid())
	{
		return;
	}

	ACharacter* Character = GetPlayerCharacter();
	if (!Character)
	{
		return;
	}

	USkeletalMeshComponent* MeshComp = Character->GetMesh();
	if (!MeshComp)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// EffectSubsystem을 통해 태그로 이펙트 스폰
	UKREffectSubsystem* EffectSubsystem = World->GetSubsystem<UKREffectSubsystem>();
	if (EffectSubsystem)
	{
		ActiveAfterImageEffect = Cast<UNiagaraComponent>(EffectSubsystem->SpawnEffectAttached(EffectTag, MeshComp, NAME_None));

		if (ActiveAfterImageEffect)
		{
			// Skeletal Mesh 파라미터 전달
			ActiveAfterImageEffect->SetVariableObject(FName("User.SourceSkeletalMesh"), MeshComp);
			ActiveAfterImageEffect->SetColorParameter(FName("Color"), GetEffectColor());

			UE_LOG(LogTemp, Log, TEXT("AerialComboTestActor: After image effect started by tag"));
		}
	}
}

void AKRAerialComboTestActor::StopAfterImageEffect()
{
	if (ActiveAfterImageEffect)
	{
		ActiveAfterImageEffect->Deactivate();
		ActiveAfterImageEffect = nullptr;
	}
}

void AKRAerialComboTestActor::SpawnSlamTrailEffect()
{
	FGameplayTag EffectTag = GetSlamTrailEffectTag();
	if (!EffectTag.IsValid())
	{
		return;
	}

	ACharacter* Character = GetPlayerCharacter();
	if (!Character)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// EffectSubsystem을 통해 태그로 이펙트 스폰 (무기 소켓에 부착)
	UKREffectSubsystem* EffectSubsystem = World->GetSubsystem<UKREffectSubsystem>();
	if (EffectSubsystem)
	{
		ActiveSlamTrailEffect = Cast<UNiagaraComponent>(EffectSubsystem->SpawnEffectAttached(EffectTag, Character->GetMesh(), FName("weapon_r")));

		if (ActiveSlamTrailEffect)
		{
			ActiveSlamTrailEffect->SetColorParameter(FName("Color"), GetEffectColor());
			UE_LOG(LogTemp, Log, TEXT("AerialComboTestActor: Slam trail effect started by tag"));
		}
	}
}

void AKRAerialComboTestActor::SpawnImpactEffect()
{
	FGameplayTag EffectTag = GetImpactEffectTag();
	if (!EffectTag.IsValid())
	{
		return;
	}

	ACharacter* Character = GetPlayerCharacter();
	if (!Character)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// 착지 지점에 충격파
	FVector ImpactLocation = Character->GetActorLocation();
	ImpactLocation.Z = SkillStartLocation.Z;  // 원래 바닥 높이

	UKREffectSubsystem* EffectSubsystem = World->GetSubsystem<UKREffectSubsystem>();
	if (EffectSubsystem)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(ImpactLocation);

		UNiagaraComponent* ImpactComp = Cast<UNiagaraComponent>(EffectSubsystem->SpawnEffectByTag(EffectTag, SpawnTransform, this));

		if (ImpactComp)
		{
			ImpactComp->SetFloatParameter(FName("Radius"), GetImpactRadius());
			ImpactComp->SetColorParameter(FName("Color"), GetEffectColor());
			UE_LOG(LogTemp, Log, TEXT("AerialComboTestActor: Impact effect spawned by tag at %s"), *ImpactLocation.ToString());
		}
	}
}

void AKRAerialComboTestActor::SpawnGroundCrackEffect()
{
	FGameplayTag EffectTag = GetGroundCrackEffectTag();
	if (!EffectTag.IsValid())
	{
		return;
	}

	ACharacter* Character = GetPlayerCharacter();
	if (!Character)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FVector CrackLocation = Character->GetActorLocation();
	CrackLocation.Z = SkillStartLocation.Z;

	UKREffectSubsystem* EffectSubsystem = World->GetSubsystem<UKREffectSubsystem>();
	if (EffectSubsystem)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(CrackLocation);

		EffectSubsystem->SpawnEffectByTag(EffectTag, SpawnTransform, this);
		UE_LOG(LogTemp, Log, TEXT("AerialComboTestActor: Ground crack effect spawned by tag"));
	}
}

void AKRAerialComboTestActor::ApplyHitStop(float Duration)
{
	if (Duration <= 0.0f)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// 글로벌 타임 딜레이션
	UGameplayStatics::SetGlobalTimeDilation(World, 0.01f);

	FTimerHandle HitStopHandle;
	World->GetTimerManager().SetTimer(
		HitStopHandle,
		[World]()
		{
			UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);
		},
		Duration * 0.01f,
		false
	);

	UE_LOG(LogTemp, Log, TEXT("AerialComboTestActor: Hit stop applied for %.3f seconds"), Duration);
}

// ─────────────────────────────────────────────────────
// 디버그
// ─────────────────────────────────────────────────────

void AKRAerialComboTestActor::DrawDebug()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// 타겟(이 Actor) 표시
	if (bDrawDebugTarget)
	{
		DrawDebugSphere(
			World,
			GetActorLocation(),
			50.0f,
			12,
			FColor::Red,
			false,
			-1.0f,
			0,
			3.0f
		);

		// 착지 예상 위치
		FVector GroundLocation = GetActorLocation();
		GroundLocation.Z = SkillStartLocation.Z;
		DrawDebugSphere(
			World,
			GroundLocation,
			GetImpactRadius(),
			16,
			FColor::Orange,
			false,
			-1.0f,
			0,
			1.0f
		);
	}

	// 페이즈 정보 표시
	if (bDrawDebugInfo)
	{
		FString PhaseStr;
		switch (CurrentPhase)
		{
		case ETestAerialComboPhase::None: PhaseStr = TEXT("None"); break;
		case ETestAerialComboPhase::Launch: PhaseStr = TEXT("Launch"); break;
		case ETestAerialComboPhase::Rising: PhaseStr = TEXT("Rising"); break;
		case ETestAerialComboPhase::Airborne: PhaseStr = TEXT("Airborne"); break;
		case ETestAerialComboPhase::Slam: PhaseStr = TEXT("Slam"); break;
		case ETestAerialComboPhase::Landing: PhaseStr = TEXT("Landing"); break;
		case ETestAerialComboPhase::Recovery: PhaseStr = TEXT("Recovery"); break;
		}

		DrawDebugString(
			World,
			GetActorLocation() + FVector(0, 0, 150),
			FString::Printf(TEXT("Phase: %s"), *PhaseStr),
			nullptr,
			FColor::White,
			0.0f,
			true
		);

		// 플레이어 위치 연결선
		ACharacter* Character = GetPlayerCharacter();
		if (Character)
		{
			DrawDebugLine(
				World,
				GetActorLocation(),
				Character->GetActorLocation(),
				FColor::Cyan,
				false,
				-1.0f,
				0,
				2.0f
			);
		}
	}
}
*/
