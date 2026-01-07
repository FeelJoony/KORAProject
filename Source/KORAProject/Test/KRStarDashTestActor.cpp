#include "KRStarDashTestActor.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "GAS/Abilities/HeroAbilities/Libraries/KRStarEffectSpawner.h"
#include "Camera/KRCameraMode_StarDash.h"
#include "Components/KRCameraComponent.h"
#include "Characters/KRHeroCharacter.h"
#include "SubSystem/KRSoundSubsystem.h"
#include "SubSystem/KREffectSubsystem.h"

AKRStarDashTestActor::AKRStarDashTestActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// 루트 컴포넌트 생성
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(Root);
}

void AKRStarDashTestActor::BeginPlay()
{
	Super::BeginPlay();

	// 자동 시작
	if (bAutoStartOnBeginPlay)
	{
		GetWorldTimerManager().SetTimer(
			AutoStartTimerHandle,
			this,
			&AKRStarDashTestActor::StartStarDash,
			AutoStartDelay,
			false
		);
	}
}

void AKRStarDashTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 디버그 드로우
	if (bDrawDebugPoints || bDrawDebugLines)
	{
		DrawDebug();
	}

	// 대시 중 캐릭터 위치 보간
	if (bIsDashing)
	{
		ACharacter* Character = GetPlayerCharacter();
		if (Character && CurrentPattern.DrawOrderPoints.IsValidIndex(CurrentDashIndex))
		{
			// 데이터 에셋에서 대시 지속 시간 가져오기
			float CurrentDashDuration = DashDuration;
			if (bUseDataAsset && StarDashData)
			{
				CurrentDashDuration = StarDashData->Timing.DashDuration;
			}

			float ElapsedTime = GetWorld()->GetTimeSeconds() - DashStartTime;
			float Alpha = FMath::Clamp(ElapsedTime / CurrentDashDuration, 0.0f, 1.0f);

			// 이징 함수 적용 (EaseOut)
			float EasedAlpha = 1.0f - FMath::Pow(1.0f - Alpha, 3.0f);

			FVector TargetLocation = CurrentPattern.DrawOrderPoints[CurrentDashIndex];
			FVector NewLocation = FMath::Lerp(DashStartLocation, TargetLocation, EasedAlpha);

			// 캐릭터 위치 업데이트
			Character->SetActorLocation(NewLocation);

			// 이동 방향 바라보기 (타겟이 아닌 이동하는 방향을 바라봄)
			FVector LookDirection = (TargetLocation - DashStartLocation).GetSafeNormal2D();
			if (!LookDirection.IsNearlyZero())
			{
				FRotator LookRotation = LookDirection.Rotation();
				Character->SetActorRotation(LookRotation);
			}

			// 대시 완료 체크
			if (Alpha >= 1.0f)
			{
				OnDashComplete();
			}
		}
	}
}

// ─────────────────────────────────────────────────────
// 메인 함수
// ─────────────────────────────────────────────────────

void AKRStarDashTestActor::StartStarDash()
{
	ACharacter* Character = GetPlayerCharacter();
	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("StarDashTestActor: Player character not found!"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("StarDashTestActor: Starting Star Dash!"));

	// 0. 플레이어 시작 위치 저장 (카메라용)
	PlayerSkillStartLocation = Character->GetActorLocation();

	// 1. 별 패턴 생성 (플레이어 위치를 Point[0]으로)
	GeneratePatternFromPlayer();

	// 2. 캐릭터 머티리얼 캐시
	CacheCharacterMaterials();

	// 3. 데이터 에셋에서 투명도 가져오기
	float CurrentDashOpacity = DashOpacity;
	if (bUseDataAsset && StarDashData)
	{
		CurrentDashOpacity = StarDashData->Visual.DashOpacity;
	}

	// 4. 캐릭터 투명화
	SetCharacterOpacity(CurrentDashOpacity);

	// 5. 잔상 이펙트 시작
	StartAfterImageEffect();

	// 6. 라인 이펙트 스폰
	SpawnLineEffect();

	// 7. 카메라 모드 활성화
	ActivateCameraMode();

	// 8. 대시 몽타주 재생 (루핑 몽타주는 여기서 한 번만 시작)
	StartDashMontage();

	// 9. 대시 시작 (Point[0]은 이미 현재 위치이므로 Point[1]부터)
	CurrentDashIndex = 1;
	bIsDashing = false;

	// 첫 대시 시작
	DashToNextPoint();
}

void AKRStarDashTestActor::StopStarDash()
{
	// 타이머 클리어
	GetWorldTimerManager().ClearTimer(DashTimerHandle);
	GetWorldTimerManager().ClearTimer(AutoStartTimerHandle);

	// 대시 중지
	bIsDashing = false;
	CurrentDashIndex = 0;

	// 대시 몽타주 중지
	StopDashMontage();

	// 잔상 이펙트 중지
	StopAfterImageEffect();

	// 캐릭터 투명도 복구
	RestoreCharacterOpacity();

	// 카메라 모드 비활성화
	DeactivateCameraMode();

	// 라인 이펙트 정리
	if (CurrentLineEffect)
	{
		CurrentLineEffect->DeactivateImmediate();
		CurrentLineEffect = nullptr;
	}

	// Lightning 액터 정리
	for (AActor* Actor : SpawnedLightningActors)
	{
		if (Actor && IsValid(Actor))
		{
			Actor->Destroy();
		}
	}
	SpawnedLightningActors.Empty();

	UE_LOG(LogTemp, Warning, TEXT("StarDashTestActor: Star Dash stopped."));
}

void AKRStarDashTestActor::SpawnLineEffectOnly()
{
	ACharacter* Character = GetPlayerCharacter();
	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("StarDashTestActor: Player character not found!"));
		return;
	}

	GeneratePatternFromPlayer();
	SpawnLineEffect();
}

// ─────────────────────────────────────────────────────
// 데이터 에셋 헬퍼
// ─────────────────────────────────────────────────────

int32 AKRStarDashTestActor::GetMaxDashCount() const
{
	if (bUseDataAsset && StarDashData)
	{
		return StarDashData->GetDashCount();
	}
	return bUse10PointStar ? 10 : 5;
}

FGameplayTag AKRStarDashTestActor::GetSlashLineEffectTag() const
{
	if (bUseDataAsset && StarDashData)
	{
		return StarDashData->Effects.SlashLineEffectTag;
	}
	return SlashLineEffectTag;
}

FGameplayTag AKRStarDashTestActor::GetAfterImageEffectTag() const
{
	if (bUseDataAsset && StarDashData)
	{
		return StarDashData->Effects.AfterImageEffectTag;
	}
	return AfterImageEffectTag;
}

void AKRStarDashTestActor::PlayDashSound(FVector Location)
{
	FGameplayTag SoundTag;
	if (bUseDataAsset && StarDashData)
	{
		SoundTag = StarDashData->Sound.DashStartSoundTag;
	}
	else
	{
		SoundTag = DashStartSoundTag;
	}

	if (SoundTag.IsValid())
	{
		if (UKRSoundSubsystem* SoundSubsystem = GetWorld()->GetSubsystem<UKRSoundSubsystem>())
		{
			SoundSubsystem->PlaySoundByTag(SoundTag, Location, this);
		}
	}
}

void AKRStarDashTestActor::PlaySlashSound(FVector Location)
{
	FGameplayTag SoundTag;
	if (bUseDataAsset && StarDashData)
	{
		SoundTag = StarDashData->Sound.SlashSoundTag;
	}
	else
	{
		SoundTag = SlashSoundTag;
	}

	if (SoundTag.IsValid())
	{
		if (UKRSoundSubsystem* SoundSubsystem = GetWorld()->GetSubsystem<UKRSoundSubsystem>())
		{
			SoundSubsystem->PlaySoundByTag(SoundTag, Location, this);
		}
	}
}

void AKRStarDashTestActor::PlayFinishSound(FVector Location)
{
	FGameplayTag SoundTag;
	if (bUseDataAsset && StarDashData)
	{
		SoundTag = StarDashData->Sound.FinishSoundTag;
	}
	else
	{
		SoundTag = FinishSoundTag;
	}

	if (SoundTag.IsValid())
	{
		if (UKRSoundSubsystem* SoundSubsystem = GetWorld()->GetSubsystem<UKRSoundSubsystem>())
		{
			SoundSubsystem->PlaySoundByTag(SoundTag, Location, this);
		}
	}
}

// ─────────────────────────────────────────────────────
// 패턴 생성
// ─────────────────────────────────────────────────────

void AKRStarDashTestActor::GeneratePatternFromPlayer()
{
	ACharacter* Character = GetPlayerCharacter();
	if (!Character)
	{
		return;
	}

	// 데이터 에셋에서 설정 가져오기
	float CurrentRadius = Radius;
	float CurrentInnerRadius = InnerRadius;
	float CurrentHeightOffset = HeightOffset;
	int32 PatternType = bUse10PointStar ? 0 : 1;

	if (bUseDataAsset && StarDashData)
	{
		CurrentRadius = StarDashData->Pattern.OuterRadius;
		CurrentInnerRadius = StarDashData->Pattern.InnerRadius;
		CurrentHeightOffset = StarDashData->Pattern.HeightOffset;
		PatternType = StarDashData->Pattern.PatternType;
	}

	// ★ 플레이어 현재 위치를 중심으로 별 패턴 생성
	FVector PlayerLocation = Character->GetActorLocation();
	FVector StarCenter = PlayerLocation + FVector(0.0f, 0.0f, CurrentHeightOffset);

	// 플레이어가 바라보는 방향을 기준으로 시작 각도 계산
	FVector ForwardDirection = Character->GetActorForwardVector().GetSafeNormal2D();
	float StartAngle = FMath::RadiansToDegrees(FMath::Atan2(ForwardDirection.Y, ForwardDirection.X));

	// 별 패턴 생성
	switch (PatternType)
	{
	case 0:  // 10포인트 별 (외곽 5개 + 내부 5개)
		CurrentPattern = UKRStarPatternLibrary::Generate10PointStarPattern(
			StarCenter,
			CurrentRadius,
			CurrentInnerRadius,  // 0이면 자동 계산
			FVector::UpVector,
			StartAngle - 90.0f  // 플레이어 전방 기준
		);
		break;

	case 1:  // 5포인트 별 (꼭짓점 스킵하며 교차선)
		CurrentPattern = UKRStarPatternLibrary::GenerateStarPattern(
			StarCenter,
			CurrentRadius,
			FVector::UpVector,
			StartAngle - 90.0f
		);
		break;

	case 2:  // 커스텀 패턴 (데이터 에셋에서)
		if (bUseDataAsset && StarDashData)
		{
			CurrentPattern.CenterLocation = StarCenter;
			CurrentPattern.Radius = CurrentRadius;
			CurrentPattern.DrawOrderPoints.Empty();
			CurrentPattern.LineStartPoints.Empty();
			CurrentPattern.LineEndPoints.Empty();

			// 첫 번째 포인트는 별의 시작점 (중심에서 Forward 방향)
			FVector FirstPoint = StarCenter + ForwardDirection * CurrentRadius;
			CurrentPattern.DrawOrderPoints.Add(FirstPoint);

			for (const FKRDashPointData& Point : StarDashData->Pattern.CustomPoints)
			{
				FVector WorldPoint = StarCenter + Point.LocalOffset;
				CurrentPattern.DrawOrderPoints.Add(WorldPoint);
			}

			// 시작점으로 돌아오기
			if (StarDashData->Pattern.bReturnToStart && CurrentPattern.DrawOrderPoints.Num() > 0)
			{
				CurrentPattern.DrawOrderPoints.Add(CurrentPattern.DrawOrderPoints[0]);
			}

			// 라인 세그먼트 생성
			for (int32 i = 0; i < CurrentPattern.DrawOrderPoints.Num() - 1; ++i)
			{
				CurrentPattern.LineStartPoints.Add(CurrentPattern.DrawOrderPoints[i]);
				CurrentPattern.LineEndPoints.Add(CurrentPattern.DrawOrderPoints[i + 1]);
			}
		}
		break;
	}

	UE_LOG(LogTemp, Warning, TEXT("StarDashTestActor: Generated pattern (type=%d) with %d points"),
		PatternType, CurrentPattern.DrawOrderPoints.Num());

	for (int32 i = 0; i < CurrentPattern.DrawOrderPoints.Num(); ++i)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Point[%d]: %s"), i, *CurrentPattern.DrawOrderPoints[i].ToString());
	}
}

// ─────────────────────────────────────────────────────
// 대시 로직
// ─────────────────────────────────────────────────────

void AKRStarDashTestActor::DashToNextPoint()
{
	// 대시 횟수 결정 (데이터 에셋 또는 개별 설정)
	int32 MaxDashCount = GetMaxDashCount();

	// 모든 포인트 완료 체크
	if (CurrentDashIndex >= MaxDashCount)
	{
		CompleteStarDash();
		return;
	}

	ExecuteDash(CurrentDashIndex);
}

void AKRStarDashTestActor::ExecuteDash(int32 PointIndex)
{
	ACharacter* Character = GetPlayerCharacter();
	if (!Character || !CurrentPattern.DrawOrderPoints.IsValidIndex(PointIndex))
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("StarDashTestActor: Executing dash to Point[%d]"), PointIndex);

	// 몽타주 상태 확인 (디버그)
	UAnimMontage* CurrentDashMontage = DashMontage;
	if (bUseDataAsset && StarDashData && StarDashData->Montage.DashMontage)
	{
		CurrentDashMontage = StarDashData->Montage.DashMontage;
	}
	if (CurrentDashMontage)
	{
		UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			bool bIsPlaying = AnimInstance->Montage_IsPlaying(CurrentDashMontage);
			UE_LOG(LogTemp, Log, TEXT("  - Montage '%s' IsPlaying: %s"),
				*CurrentDashMontage->GetName(), bIsPlaying ? TEXT("YES") : TEXT("NO"));
		}
	}

	// 대시 시작 정보 저장
	DashStartLocation = Character->GetActorLocation();
	DashStartTime = GetWorld()->GetTimeSeconds();
	bIsDashing = true;

	// 대시 사운드 재생
	PlayDashSound(DashStartLocation);

	// Lightning 이펙트 스폰 (대시 라인에)
	FVector TargetLocation = CurrentPattern.DrawOrderPoints[PointIndex];
	SpawnLightningForDash(DashStartLocation, TargetLocation);

	// 몽타주는 StartDashMontage()에서 한 번만 재생 (루핑)
}

void AKRStarDashTestActor::OnDashComplete()
{
	bIsDashing = false;

	UE_LOG(LogTemp, Log, TEXT("StarDashTestActor: Dash to Point[%d] complete"), CurrentDashIndex);

	// 슬래시 사운드 재생 (대시 완료 위치에서)
	ACharacter* Character = GetPlayerCharacter();
	if (Character)
	{
		PlaySlashSound(Character->GetActorLocation());
	}

	// 다음 포인트로
	CurrentDashIndex++;

	// 데이터 에셋에서 딜레이 가져오기
	float CurrentDashInterval = DashInterval;
	if (bUseDataAsset && StarDashData)
	{
		CurrentDashInterval = StarDashData->Timing.DashInterval;
	}

	// 딜레이 후 다음 대시
	GetWorldTimerManager().SetTimer(
		DashTimerHandle,
		this,
		&AKRStarDashTestActor::DashToNextPoint,
		CurrentDashInterval,
		false
	);
}

void AKRStarDashTestActor::CompleteStarDash()
{
	UE_LOG(LogTemp, Warning, TEXT("StarDashTestActor: Star Dash complete!"));

	// 피니시 사운드 재생
	ACharacter* Character = GetPlayerCharacter();
	if (Character)
	{
		PlayFinishSound(Character->GetActorLocation());
	}

	// 대시 몽타주 중지 후 복귀 몽타주 재생
	StopDashMontage();
	PlayFinishMontage();

	// 잔상 이펙트 중지
	StopAfterImageEffect();

	// 투명도 복구
	RestoreCharacterOpacity();

	// 카메라 모드 비활성화
	DeactivateCameraMode();

	// 상태 초기화
	bIsDashing = false;
	CurrentDashIndex = 0;

	// Lightning 액터 정리 (SetLifeSpan으로 자동 삭제되므로 배열만 비움)
	SpawnedLightningActors.Empty();
}

// ─────────────────────────────────────────────────────
// 캐릭터 제어
// ─────────────────────────────────────────────────────

ACharacter* AKRStarDashTestActor::GetPlayerCharacter() const
{
	return UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

void AKRStarDashTestActor::CacheCharacterMaterials()
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

	CachedMaterials.Empty();
	OriginalMaterials.Empty();

	int32 NumMaterials = Mesh->GetNumMaterials();
	for (int32 i = 0; i < NumMaterials; ++i)
	{
		UMaterialInterface* OriginalMaterial = Mesh->GetMaterial(i);
		OriginalMaterials.Add(OriginalMaterial);

		UMaterialInstanceDynamic* DynMaterial = Mesh->CreateDynamicMaterialInstance(i, OriginalMaterial);
		CachedMaterials.Add(DynMaterial);
	}
}

void AKRStarDashTestActor::SetCharacterOpacity(float Opacity)
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

	// 데이터 에셋에서 비주얼 설정 가져오기
	bool CurrentUseVisibilityHide = bUseVisibilityHide;
	bool CurrentUseDitheredOpacity = bUseDitheredOpacity;
	int32 CurrentOpacityPrimitiveDataIndex = OpacityPrimitiveDataIndex;
	FName CurrentOpacityParameterName = OpacityParameterName;

	if (bUseDataAsset && StarDashData)
	{
		CurrentUseVisibilityHide = StarDashData->Visual.bUseVisibilityHide;
		CurrentUseDitheredOpacity = StarDashData->Visual.bUseDitheredOpacity;
		CurrentOpacityPrimitiveDataIndex = StarDashData->Visual.OpacityPrimitiveDataIndex;
		CurrentOpacityParameterName = StarDashData->Visual.OpacityParameterName;
	}

	// 방법 1: Visibility 숨기기 (가장 간단, 완전히 숨겨짐)
	if (CurrentUseVisibilityHide)
	{
		Mesh->SetVisibility(Opacity > 0.5f);
		return;
	}

	// 방법 2: CustomPrimitiveData 사용 (Dithered Opacity - 머티리얼 설정 필요)
	if (CurrentUseDitheredOpacity)
	{
		// CustomPrimitiveData로 투명도 전달
		// 머티리얼에서 CustomPrimitiveData[Index]를 읽어 DitherTemporalAA에 연결해야 함
		Mesh->SetCustomPrimitiveDataFloat(CurrentOpacityPrimitiveDataIndex, Opacity);

		UE_LOG(LogTemp, Log, TEXT("StarDashTestActor: Set CustomPrimitiveData[%d] = %.2f"),
			CurrentOpacityPrimitiveDataIndex, Opacity);
	}

	// 방법 3: 머티리얼 파라미터 직접 설정 (머티리얼이 Translucent/Masked여야 함)
	for (UMaterialInstanceDynamic* DynMaterial : CachedMaterials)
	{
		if (DynMaterial)
		{
			DynMaterial->SetScalarParameterValue(CurrentOpacityParameterName, Opacity);
		}
	}
}

void AKRStarDashTestActor::RestoreCharacterOpacity()
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

	// 데이터 에셋에서 비주얼 설정 가져오기
	bool CurrentUseVisibilityHide = bUseVisibilityHide;
	bool CurrentUseDitheredOpacity = bUseDitheredOpacity;
	int32 CurrentOpacityPrimitiveDataIndex = OpacityPrimitiveDataIndex;

	if (bUseDataAsset && StarDashData)
	{
		CurrentUseVisibilityHide = StarDashData->Visual.bUseVisibilityHide;
		CurrentUseDitheredOpacity = StarDashData->Visual.bUseDitheredOpacity;
		CurrentOpacityPrimitiveDataIndex = StarDashData->Visual.OpacityPrimitiveDataIndex;
	}

	// Visibility 복구
	if (CurrentUseVisibilityHide)
	{
		Mesh->SetVisibility(true);
	}

	// CustomPrimitiveData 복구 (1.0 = 불투명)
	if (CurrentUseDitheredOpacity)
	{
		Mesh->SetCustomPrimitiveDataFloat(CurrentOpacityPrimitiveDataIndex, 1.0f);
	}

	// 원래 머티리얼로 복구
	for (int32 i = 0; i < OriginalMaterials.Num(); ++i)
	{
		if (OriginalMaterials[i])
		{
			Mesh->SetMaterial(i, OriginalMaterials[i]);
		}
	}

	CachedMaterials.Empty();
	OriginalMaterials.Empty();
}

void AKRStarDashTestActor::StartAfterImageEffect()
{
	// 잔상 이펙트 태그와 색상 가져오기
	FGameplayTag CurrentAfterImageTag = GetAfterImageEffectTag();
	FLinearColor CurrentAfterImageColor = AfterImageColor;

	if (bUseDataAsset && StarDashData)
	{
		CurrentAfterImageColor = StarDashData->Effects.AfterImageColor;
	}

	if (!CurrentAfterImageTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("StarDashTestActor: No AfterImageEffectTag set!"));
		return;
	}

	ACharacter* Character = GetPlayerCharacter();
	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("StarDashTestActor: No Character for AfterImage!"));
		return;
	}

	USkeletalMeshComponent* MeshComp = Character->GetMesh();
	if (!MeshComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("StarDashTestActor: No MeshComp for AfterImage!"));
		return;
	}

	// EffectSubsystem을 통해 태그로 이펙트 스폰
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UKREffectSubsystem* EffectSubsystem = World->GetSubsystem<UKREffectSubsystem>();
	if (EffectSubsystem)
	{
		// 캐릭터 메쉬에 잔상 이펙트 부착
		ActiveAfterImageEffect = Cast<UNiagaraComponent>(EffectSubsystem->SpawnEffectAttached(
			CurrentAfterImageTag,
			MeshComp,
			NAME_None
		));

		if (ActiveAfterImageEffect)
		{
			UE_LOG(LogTemp, Log, TEXT("StarDashTestActor: AfterImage spawned by tag: %s"), *CurrentAfterImageTag.ToString());

			// Skeletal Mesh 파라미터 전달 (정확한 이름: User.SourceSkeletalMesh)
			ActiveAfterImageEffect->SetVariableObject(FName("User.SourceSkeletalMesh"), MeshComp);

			// 잔상 색상 설정 - 불투명하게
			FLinearColor OpaqueColor = CurrentAfterImageColor;
			OpaqueColor.A = 1.0f;
			ActiveAfterImageEffect->SetColorParameter(FName("Color"), OpaqueColor);
			ActiveAfterImageEffect->SetColorParameter(FName("User.Color"), OpaqueColor);
			ActiveAfterImageEffect->SetColorParameter(FName("AfterImageColor"), OpaqueColor);

			// 잔상 생존 시간 설정
			ActiveAfterImageEffect->SetFloatParameter(FName("Lifetime"), 0.5f);
			ActiveAfterImageEffect->SetFloatParameter(FName("User.Lifetime"), 0.5f);
			ActiveAfterImageEffect->SetFloatParameter(FName("Duration"), 0.5f);
			ActiveAfterImageEffect->SetFloatParameter(FName("FadeTime"), 0.3f);

			// 스폰 레이트 설정
			ActiveAfterImageEffect->SetFloatParameter(FName("SpawnRate"), 30.0f);
			ActiveAfterImageEffect->SetFloatParameter(FName("User.SpawnRate"), 30.0f);
			ActiveAfterImageEffect->SetIntParameter(FName("SpawnCount"), 5);

			// 투명도/알파 설정
			ActiveAfterImageEffect->SetFloatParameter(FName("Opacity"), 0.8f);
			ActiveAfterImageEffect->SetFloatParameter(FName("User.Opacity"), 0.8f);
			ActiveAfterImageEffect->SetFloatParameter(FName("Alpha"), 0.8f);

			UE_LOG(LogTemp, Log, TEXT("StarDashTestActor: AfterImage params set - Color: %s"), *OpaqueColor.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("StarDashTestActor: Failed to spawn AfterImage by tag!"));
		}
	}
}

void AKRStarDashTestActor::StopAfterImageEffect()
{
	if (ActiveAfterImageEffect)
	{
		ActiveAfterImageEffect->Deactivate();
		ActiveAfterImageEffect = nullptr;
	}
}

// ─────────────────────────────────────────────────────
// 몽타주 제어
// ─────────────────────────────────────────────────────

void AKRStarDashTestActor::StartDashMontage()
{
	ACharacter* Character = GetPlayerCharacter();
	if (!Character)
	{
		UE_LOG(LogTemp, Error, TEXT("StarDashTestActor::StartDashMontage - Character is NULL!"));
		return;
	}

	// 데이터 에셋에서 몽타주 설정 가져오기
	UAnimMontage* CurrentDashMontage = DashMontage;
	float CurrentMontagePlayRate = MontagePlayRate;

	if (bUseDataAsset && StarDashData)
	{
		if (StarDashData->Montage.DashMontage)
		{
			CurrentDashMontage = StarDashData->Montage.DashMontage;
		}
		CurrentMontagePlayRate = StarDashData->Montage.DashMontagePlayRate;
	}

	if (!CurrentDashMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("StarDashTestActor: No DashMontage to play!"));
		return;
	}

	USkeletalMeshComponent* Mesh = Character->GetMesh();
	if (!Mesh)
	{
		UE_LOG(LogTemp, Error, TEXT("StarDashTestActor::StartDashMontage - Mesh is NULL!"));
		return;
	}

	UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("StarDashTestActor::StartDashMontage - AnimInstance is NULL!"));
		return;
	}

	// 몽타주 정보 로깅
	UE_LOG(LogTemp, Log, TEXT("StarDashTestActor: Attempting to play montage '%s'"), *CurrentDashMontage->GetName());
	UE_LOG(LogTemp, Log, TEXT("  - Montage Length: %.3f"), CurrentDashMontage->GetPlayLength());
	UE_LOG(LogTemp, Log, TEXT("  - Montage bLoop: %s"), CurrentDashMontage->bLoop ? TEXT("true") : TEXT("false"));

	// 몽타주 재생 (루핑 몽타주는 계속 반복됨)
	// 블렌드 시간 -1.0f = 몽타주 기본 BlendIn 설정 사용
	float Duration = AnimInstance->Montage_Play(CurrentDashMontage, CurrentMontagePlayRate);

	if (Duration > 0.0f)
	{
		UE_LOG(LogTemp, Log, TEXT("StarDashTestActor: SUCCESS - Montage '%s' playing (Duration: %.3f, Rate: %.2f)"),
			*CurrentDashMontage->GetName(), Duration, CurrentMontagePlayRate);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("StarDashTestActor: FAILED - Montage_Play returned %.3f"), Duration);
		UE_LOG(LogTemp, Error, TEXT("  - Check if AnimBlueprint has a DefaultSlot node"));
	}
}

void AKRStarDashTestActor::StopDashMontage()
{
	ACharacter* Character = GetPlayerCharacter();
	if (!Character)
	{
		return;
	}

	// 데이터 에셋에서 몽타주 가져오기
	UAnimMontage* CurrentDashMontage = DashMontage;
	if (bUseDataAsset && StarDashData && StarDashData->Montage.DashMontage)
	{
		CurrentDashMontage = StarDashData->Montage.DashMontage;
	}

	if (!CurrentDashMontage)
	{
		return;
	}

	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (AnimInstance && AnimInstance->Montage_IsPlaying(CurrentDashMontage))
	{
		// 블렌드 없이 즉시 정지 (짧은 루핑 애니메이션)
		AnimInstance->Montage_Stop(0.0f, CurrentDashMontage);
		UE_LOG(LogTemp, Log, TEXT("StarDashTestActor: Stopped dash montage '%s'"), *CurrentDashMontage->GetName());
	}
}

void AKRStarDashTestActor::PlayFinishMontage()
{
	ACharacter* Character = GetPlayerCharacter();
	if (!Character)
	{
		return;
	}

	// 데이터 에셋에서 복귀 몽타주 가져오기
	UAnimMontage* FinishMontage = nullptr;
	float FinishMontagePlayRate = 1.0f;

	if (bUseDataAsset && StarDashData)
	{
		FinishMontage = StarDashData->Montage.FinishMontage;
		FinishMontagePlayRate = StarDashData->Montage.FinishMontagePlayRate;
	}

	if (!FinishMontage)
	{
		UE_LOG(LogTemp, Log, TEXT("StarDashTestActor: No FinishMontage set, skipping"));
		return;
	}

	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		// 복귀 몽타주 재생 (블렌드 없이 즉시)
		float Duration = AnimInstance->Montage_Play(FinishMontage, FinishMontagePlayRate, EMontagePlayReturnType::MontageLength, 0.0f);
		UE_LOG(LogTemp, Log, TEXT("StarDashTestActor: Playing finish montage '%s' (Duration: %.3f)"),
			*FinishMontage->GetName(), Duration);
	}
}

// ─────────────────────────────────────────────────────
// 라인 이펙트
// ─────────────────────────────────────────────────────

void AKRStarDashTestActor::SpawnLineEffect()
{
	// 슬래시 라인 이펙트 태그 가져오기
	FGameplayTag CurrentSlashLineTag = GetSlashLineEffectTag();

	if (!CurrentSlashLineTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("StarDashTestActor: SlashLineEffectTag is not set!"));
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// 기존 이펙트 정리
	if (CurrentLineEffect)
	{
		CurrentLineEffect->DeactivateImmediate();
		CurrentLineEffect = nullptr;
	}

	// 데이터 에셋에서 설정 가져오기
	float CurrentLineDrawDuration = LineDrawDuration;
	FLinearColor CurrentLineColor = LineColor;
	float CurrentLineWidth = LineWidth;
	float CurrentGlowIntensity = GlowIntensity;

	if (bUseDataAsset && StarDashData)
	{
		CurrentLineDrawDuration = StarDashData->Timing.LineDrawDuration;
		CurrentLineColor = StarDashData->Effects.LineColor;
		CurrentLineWidth = StarDashData->Effects.LineWidth;
		CurrentGlowIntensity = StarDashData->Effects.GlowIntensity;
	}

	// EffectSubsystem을 통해 태그로 이펙트 스폰
	UKREffectSubsystem* EffectSubsystem = World->GetSubsystem<UKREffectSubsystem>();
	if (EffectSubsystem)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(CurrentPattern.CenterLocation);

		CurrentLineEffect = Cast<UNiagaraComponent>(EffectSubsystem->SpawnEffectByTag(CurrentSlashLineTag, SpawnTransform, this));

		if (CurrentLineEffect)
		{
			// 파라미터 설정
			CurrentLineEffect->SetFloatParameter(FName("User.DrawDuration"), CurrentLineDrawDuration);
			CurrentLineEffect->SetColorParameter(FName("User.LineColor"), CurrentLineColor);
			CurrentLineEffect->SetFloatParameter(FName("User.LineWidth"), CurrentLineWidth);
			CurrentLineEffect->SetFloatParameter(FName("User.GlowIntensity"), CurrentGlowIntensity);

			// Points 배열 전달
			UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayPosition(
				CurrentLineEffect,
				FName("User.Points"),
				CurrentPattern.DrawOrderPoints
			);

			UE_LOG(LogTemp, Log, TEXT("StarDashTestActor: Line effect spawned by tag with %d points"),
				CurrentPattern.DrawOrderPoints.Num());
		}
	}
}

void AKRStarDashTestActor::SpawnLightningForDash(FVector StartPoint, FVector EndPoint)
{
	// Lightning 활성화 체크 (데이터 에셋 또는 개별 설정)
	bool bLightningEnabled = bEnableLightning;
	if (bUseDataAsset && StarDashData)
	{
		bLightningEnabled = StarDashData->Effects.bEnableLightning;
	}

	if (!bLightningEnabled || !SplineLightningClass)
	{
		return;
	}

	// 라인 인덱스를 seed로 사용하여 각 별 변마다 다른 패턴 생성
	int32 Seed = CurrentDashIndex + FMath::Rand() % 1000 * 100;

	// SplineVFX BP Actor로 Lightning 스폰
	AActor* LightningActor = UKRStarEffectSpawner::SpawnSplineLightningActor(
		this,
		SplineLightningClass,
		StartPoint,
		EndPoint,
		2.0f,  // 수명 2초
		Seed
	);

	if (LightningActor)
	{
		SpawnedLightningActors.Add(LightningActor);
		UE_LOG(LogTemp, Log, TEXT("StarDashTestActor: Spline Lightning Actor spawned (Seed=%d)"), Seed);
	}
}

// ─────────────────────────────────────────────────────
// 디버그
// ─────────────────────────────────────────────────────

void AKRStarDashTestActor::DrawDebug()
{
	if (CurrentPattern.DrawOrderPoints.Num() == 0)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// 타겟(중심) 표시
	DrawDebugSphere(
		World,
		GetActorLocation(),
		30.0f,
		12,
		FColor::Red,
		false,
		-1.0f,
		0,
		3.0f
	);

	// 별 꼭짓점 표시
	if (bDrawDebugPoints)
	{
		for (int32 i = 0; i < CurrentPattern.DrawOrderPoints.Num(); ++i)
		{
			FColor PointColor = (i == 0) ? FColor::Green : FColor::Blue;
			DrawDebugSphere(
				World,
				CurrentPattern.DrawOrderPoints[i],
				15.0f,
				8,
				PointColor,
				false,
				-1.0f,
				0,
				2.0f
			);

			DrawDebugString(
				World,
				CurrentPattern.DrawOrderPoints[i] + FVector(0, 0, 30),
				FString::Printf(TEXT("%d"), i),
				nullptr,
				FColor::White,
				0.0f,
				true
			);
		}
	}

	// 별 라인 표시
	if (bDrawDebugLines)
	{
		FColor StarColor = FColor(
			FMath::Clamp(int32(LineColor.R * 255), 0, 255),
			FMath::Clamp(int32(LineColor.G * 255), 0, 255),
			FMath::Clamp(int32(LineColor.B * 255), 0, 255)
		);

		for (int32 i = 0; i < CurrentPattern.DrawOrderPoints.Num() - 1; ++i)
		{
			DrawDebugLine(
				World,
				CurrentPattern.DrawOrderPoints[i],
				CurrentPattern.DrawOrderPoints[i + 1],
				StarColor,
				false,
				-1.0f,
				0,
				2.0f
			);
		}
	}
}

// ─────────────────────────────────────────────────────
// 카메라 제어
// ─────────────────────────────────────────────────────

TSubclassOf<UKRCameraMode> AKRStarDashTestActor::GetCameraModeClass() const
{
	// 데이터 에셋 우선
	if (bUseDataAsset && StarDashData && StarDashData->Camera.StarDashCameraMode)
	{
		return StarDashData->Camera.StarDashCameraMode;
	}

	// 개별 설정
	return CameraModeClass;
}

void AKRStarDashTestActor::ActivateCameraMode()
{
	// 카메라 모드 활성화 체크
	bool bCameraEnabled = bEnableCameraMode;
	if (bUseDataAsset && StarDashData)
	{
		bCameraEnabled = StarDashData->Camera.bEnableCameraMode;
	}

	if (!bCameraEnabled)
	{
		return;
	}

	// 카메라 모드 클래스 가져오기
	TSubclassOf<UKRCameraMode> ModeClass = GetCameraModeClass();
	if (!ModeClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("StarDashTestActor: Camera mode class not set!"));
		return;
	}

	// 플레이어 캐릭터에서 카메라 컴포넌트 가져오기
	ACharacter* Character = GetPlayerCharacter();
	if (!Character)
	{
		return;
	}

	// KRHeroCharacter 캐스팅
	AKRHeroCharacter* HeroCharacter = Cast<AKRHeroCharacter>(Character);
	if (!HeroCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("StarDashTestActor: Player is not KRHeroCharacter!"));
		return;
	}

	// 카메라 컴포넌트 가져오기
	UKRCameraComponent* CameraComp = HeroCharacter->GetCameraComponent();
	if (!CameraComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("StarDashTestActor: Camera component not found!"));
		return;
	}

	// 기본 카메라 모드 업데이트 잠금 (먼저!)
	CameraComp->SetCameraModeUpdateLocked(true);

	// 카메라 모드 Push
	CameraComp->PushCameraMode(ModeClass);
	ActiveCameraModeClass = ModeClass;

	// StarDash 카메라 모드 설정
	// 주의: Blueprint 서브클래스 사용 시에도 올바른 인스턴스를 가져오도록 ModeClass 사용
	UKRCameraMode* CameraModeInstance = CameraComp->GetCameraModeInstanceByClass(ModeClass);
	UKRCameraMode_StarDash* StarDashCameraModeInstance = Cast<UKRCameraMode_StarDash>(CameraModeInstance);
	if (StarDashCameraModeInstance)
	{
		// 타겟 위치 = TestActor 위치 (적 위치 역할)
		// 플레이어 시작 위치 = 스킬 시작 시 저장한 위치
		StarDashCameraModeInstance->SetSkillLocations(GetActorLocation(), PlayerSkillStartLocation);
		StarDashCameraModeInstance->SetPlayerActor(Character);

		UE_LOG(LogTemp, Log, TEXT("StarDashTestActor: Camera mode configured - Target: %s, PlayerStart: %s, CameraLocked: true"),
			*GetActorLocation().ToString(), *PlayerSkillStartLocation.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("StarDashTestActor: Failed to get StarDash camera mode instance!"));
	}

	UE_LOG(LogTemp, Log, TEXT("StarDashTestActor: Camera mode '%s' pushed"), *ModeClass->GetName());
}

void AKRStarDashTestActor::DeactivateCameraMode()
{
	if (!ActiveCameraModeClass)
	{
		return;
	}

	// 플레이어 캐릭터에서 카메라 컴포넌트 가져오기
	ACharacter* Character = GetPlayerCharacter();
	if (!Character)
	{
		ActiveCameraModeClass = nullptr;
		return;
	}

	// KRHeroCharacter 캐스팅
	AKRHeroCharacter* HeroCharacter = Cast<AKRHeroCharacter>(Character);
	if (!HeroCharacter)
	{
		ActiveCameraModeClass = nullptr;
		return;
	}

	// 카메라 컴포넌트 가져오기
	UKRCameraComponent* CameraComp = HeroCharacter->GetCameraComponent();
	if (!CameraComp)
	{
		ActiveCameraModeClass = nullptr;
		return;
	}

	// 카메라 모드 Remove
	CameraComp->RemoveCameraMode(ActiveCameraModeClass);
	CameraComp->SetCameraModeUpdateLocked(false);

	UE_LOG(LogTemp, Log, TEXT("StarDashTestActor: Camera mode '%s' removed, CameraUnlocked"), *ActiveCameraModeClass->GetName());

	ActiveCameraModeClass = nullptr;
}
