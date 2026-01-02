#include "KREffectSubsystem.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

bool FKRActiveEffectInstance::ShouldDestroy(float CurrentTime) const
{
	if (!bAutoDestroy || Lifetime <= 0.0f)
	{
		return false;
	}

	return (CurrentTime - SpawnTime) >= Lifetime;
}

void UKREffectSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	CurrentQualityLevel = EKREffectQualityLevel::High;
	MaxActiveEffects = 100;
}

void UKREffectSubsystem::Deinitialize()
{
	StopAllEffects(true);
	EffectPools.Empty();
	Super::Deinitialize();
}

void UKREffectSubsystem::Tick(float DeltaTime)
{
	CleanupInactiveEffects();
}

TStatId UKREffectSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UKREffectSubsystem, STATGROUP_Tickables);
}

UFXSystemComponent* UKREffectSubsystem::SpawnEffectByTag(const FGameplayTag& EffectTag, const FTransform& Transform, AActor* Owner)
{
	UKREffectDefinition* EffectDef = GetEffectDefinition(EffectTag);
	if (!EffectDef)
	{
		UE_LOG(LogTemp, Warning, TEXT("Effect Def Null"));
		return nullptr;
	}

	return SpawnEffect(EffectDef, Transform, Owner);
}

UFXSystemComponent* UKREffectSubsystem::SpawnEffectAttached(const FGameplayTag& EffectTag, USceneComponent* AttachToComponent, FName SocketName, const FVector& LocationOffset, const FRotator& RotationOffset)
{
	if (!AttachToComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("  FAILED: AttachToComponent is null"));
		return nullptr;
	}

	UKREffectDefinition* EffectDef = GetEffectDefinition(EffectTag);
	if (!EffectDef)
	{
		UE_LOG(LogTemp, Error, TEXT("  FAILED: EffectDefinition not found for tag %s"), *EffectTag.ToString());
		return nullptr;
	}

	FKRActiveEffectInstance Instance;
	Instance.EffectSystemType = EffectDef->EffectSystemType;
	Instance.EffectTag = EffectTag;
	Instance.SpawnTime = GetWorld()->GetTimeSeconds();
	Instance.Lifetime = EffectDef->SpawnSettings.Lifetime;
	Instance.bAutoDestroy = EffectDef->SpawnSettings.bAutoDestroy;

	UFXSystemComponent* ResultComponent = nullptr;

	if (EffectDef->EffectSystemType == EKREffectSystemType::Niagara)
	{
		UNiagaraSystem* NiagaraSystem = EffectDef->NiagaraEffect.LoadSynchronous();
		if (!NiagaraSystem)
		{
			UE_LOG(LogTemp, Error, TEXT("  FAILED: NiagaraSystem is null (could not load)"));
			return nullptr;
		}
		
		UNiagaraComponent* Component = UNiagaraFunctionLibrary::SpawnSystemAttached(
			NiagaraSystem,
			AttachToComponent,
			SocketName,
			LocationOffset,
			RotationOffset,
			EffectDef->SpawnSettings.Scale,
			EAttachLocation::SnapToTarget,
			false,  // bAutoDestroy - 우리가 관리
			ENCPoolMethod::None,
			true    // bAutoActivate - 즉시 활성화
		);

		if (!Component)
		{
			UE_LOG(LogTemp, Error, TEXT("  FAILED: SpawnSystemAttached returned null"));
			return nullptr;
		}
		Instance.NiagaraComponent = Component;
		ResultComponent = Component;
	}
	else
	{
		UParticleSystem* CascadeSystem = EffectDef->CascadeEffect.LoadSynchronous();
		if (!CascadeSystem)
		{
			UE_LOG(LogTemp, Error, TEXT("  FAILED: CascadeSystem is null (could not load)"));
			return nullptr;
		}
		
		UParticleSystemComponent* Component = UGameplayStatics::SpawnEmitterAttached(
			CascadeSystem,
			AttachToComponent,
			SocketName,
			LocationOffset,
			RotationOffset,
			EffectDef->SpawnSettings.Scale,
			EAttachLocation::SnapToTarget,
			false,  // bAutoDestroy - 우리가 관리
			EPSCPoolMethod::None,
			true    // bAutoActivate
		);

		if (!Component)
		{
			UE_LOG(LogTemp, Error, TEXT("  FAILED: SpawnEmitterAttached returned null"));
			return nullptr;
		}
		Instance.CascadeComponent = Component;
		ResultComponent = Component;
	}

	ActiveEffects.Add(Instance);
	return ResultComponent;
}

UFXSystemComponent* UKREffectSubsystem::SpawnEffect(const UKREffectDefinition* EffectDef, const FTransform& Transform, AActor* Owner)
{
	if (!EffectDef || !CanSpawnEffect(EffectDef, Transform.GetLocation()))
	{
		return nullptr;
	}

	if (ActiveEffects.Num() >= MaxActiveEffects)
	{
		RemoveLowPriorityEffects(1);
	}

	FKRActiveEffectInstance Instance;
	Instance.EffectSystemType = EffectDef->EffectSystemType;
	Instance.EffectTag = EffectDef->EffectTag;
	Instance.SpawnTime = GetWorld()->GetTimeSeconds();
	Instance.Lifetime = EffectDef->SpawnSettings.Lifetime;
	Instance.bAutoDestroy = EffectDef->SpawnSettings.bAutoDestroy;

	UFXSystemComponent* ResultComponent = nullptr;

	if (EffectDef->EffectSystemType == EKREffectSystemType::Niagara)
	{
		UNiagaraComponent* Component = AcquireNiagaraComponentFromPool(EffectDef);
		if (!Component)
		{
			return nullptr;
		}

		ConfigureNiagaraComponent(Component, EffectDef, Transform);
		Component->Activate(true);

		Instance.NiagaraComponent = Component;
		ResultComponent = Component;
	}
	else
	{
		UParticleSystemComponent* Component = AcquireCascadeComponentFromPool(EffectDef);
		if (!Component)
		{
			return nullptr;
		}

		ConfigureCascadeComponent(Component, EffectDef, Transform);
		Component->Activate(true);

		Instance.CascadeComponent = Component;
		ResultComponent = Component;
	}

	ActiveEffects.Add(Instance);
	return ResultComponent;
}

void UKREffectSubsystem::StopEffectByTag(const FGameplayTag& EffectTag, bool bImmediate)
{
	for (int32 i = ActiveEffects.Num() - 1; i >= 0; --i)
	{
		if (ActiveEffects[i].EffectTag.MatchesTagExact(EffectTag) && ActiveEffects[i].IsValid())
		{
			UFXSystemComponent* Component = ActiveEffects[i].GetFXComponent();
			if (Component)
			{
				if (bImmediate)
				{
					Component->DestroyComponent();
				}
				else
				{
					Component->Deactivate();
				}
			}
		}
	}
}

void UKREffectSubsystem::StopAllEffects(bool bImmediate)
{
	for (FKRActiveEffectInstance& Instance : ActiveEffects)
	{
		if (Instance.IsValid())
		{
			UFXSystemComponent* Component = Instance.GetFXComponent();
			if (Component)
			{
				if (bImmediate)
				{
					Component->DestroyComponent();
				}
				else
				{
					Component->Deactivate();
				}
			}
		}
	}
	ActiveEffects.Empty();
}

void UKREffectSubsystem::StopEffectsByCategory(const FGameplayTagContainer& CategoryTags, bool bImmediate)
{
	for (int32 i = ActiveEffects.Num() - 1; i >= 0; --i)
	{
		if (ActiveEffects[i].IsValid())
		{
			UKREffectDefinition* EffectDef = GetEffectDefinition(ActiveEffects[i].EffectTag);
			if (EffectDef && EffectDef->CategoryTags.HasAny(CategoryTags))
			{
				UFXSystemComponent* Component = ActiveEffects[i].GetFXComponent();
				if (Component)
				{
					if (bImmediate)
					{
						Component->DestroyComponent();
					}
					else
					{
						Component->Deactivate();
					}
				}
			}
		}
	}
}

void UKREffectSubsystem::RegisterEffectDefinition(UKREffectDefinition* EffectDef)
{
	if (EffectDef && EffectDef->EffectTag.IsValid())
	{
		EffectDefinitions.Add(EffectDef->EffectTag, EffectDef);
		
		if (EffectDef->PoolingSettings.bUsePooling)
		{
			InitializePool(EffectDef);
		}
	}
}

UKREffectDefinition* UKREffectSubsystem::GetEffectDefinition(const FGameplayTag& EffectTag) const
{
	if (const TObjectPtr<UKREffectDefinition>* Found = EffectDefinitions.Find(EffectTag))
	{
		return *Found;
	}
	return nullptr;
}

void UKREffectSubsystem::SetEffectQualityLevel(EKREffectQualityLevel QualityLevel)
{
	CurrentQualityLevel = QualityLevel;
}

void UKREffectSubsystem::SetMaxEffectCount(int32 MaxCount)
{
	MaxActiveEffects = FMath::Max(MaxCount, 1);
}

UNiagaraComponent* UKREffectSubsystem::AcquireNiagaraComponentFromPool(const UKREffectDefinition* EffectDef)
{
	if (!EffectDef)
	{
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	UNiagaraSystem* NiagaraSystem = EffectDef->NiagaraEffect.LoadSynchronous();
	if (!NiagaraSystem)
	{
		return nullptr;
	}

	if (!EffectDef->PoolingSettings.bUsePooling)
	{
		UNiagaraComponent* Component = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			World,
			NiagaraSystem,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			FVector::OneVector,
			false,  // bAutoDestroy - 우리가 직접 관리
			false,  // bAutoActivate
			ENCPoolMethod::None,
			true    // bPreCullCheck
		);
		return Component;
	}

	FKREffectPool* Pool = EffectPools.Find(EffectDef->EffectTag);
	if (!Pool)
	{
		InitializePool(EffectDef);
		Pool = EffectPools.Find(EffectDef->EffectTag);
	}

	if (!Pool)
	{
		return nullptr;
	}

	if (Pool->AvailableNiagaraComponents.Num() > 0)
	{
		UNiagaraComponent* Component = Pool->AvailableNiagaraComponents.Pop();
		Pool->ActiveNiagaraComponents.Add(Component);
		return Component;
	}

	if (Pool->ActiveNiagaraComponents.Num() >= Pool->MaxPoolSize)
	{
		return nullptr;
	}

	UNiagaraComponent* NewComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		World,
		NiagaraSystem,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		FVector::OneVector,
		false,  // bAutoDestroy
		false,  // bAutoActivate
		ENCPoolMethod::None,
		true
	);
	if (NewComponent)
	{
		Pool->ActiveNiagaraComponents.Add(NewComponent);
	}

	return NewComponent;
}

UParticleSystemComponent* UKREffectSubsystem::AcquireCascadeComponentFromPool(const UKREffectDefinition* EffectDef)
{
	if (!EffectDef)
	{
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	UParticleSystem* CascadeSystem = EffectDef->CascadeEffect.LoadSynchronous();
	if (!CascadeSystem)
	{
		return nullptr;
	}

	if (!EffectDef->PoolingSettings.bUsePooling)
	{
		UParticleSystemComponent* Component = UGameplayStatics::SpawnEmitterAtLocation(
			World,
			CascadeSystem,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			FVector::OneVector,
			false,  // bAutoDestroy - 우리가 직접 관리
			EPSCPoolMethod::None,
			false   // bAutoActivate
		);
		return Component;
	}

	FKREffectPool* Pool = EffectPools.Find(EffectDef->EffectTag);
	if (!Pool)
	{
		InitializePool(EffectDef);
		Pool = EffectPools.Find(EffectDef->EffectTag);
	}

	if (!Pool)
	{
		return nullptr;
	}

	if (Pool->AvailableCascadeComponents.Num() > 0)
	{
		UParticleSystemComponent* Component = Pool->AvailableCascadeComponents.Pop();
		Pool->ActiveCascadeComponents.Add(Component);
		return Component;
	}

	if (Pool->ActiveCascadeComponents.Num() >= Pool->MaxPoolSize)
	{
		return nullptr;
	}

	UParticleSystemComponent* NewComponent = UGameplayStatics::SpawnEmitterAtLocation(
		World,
		CascadeSystem,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		FVector::OneVector,
		false,
		EPSCPoolMethod::None,
		false  // bAutoActivate
	);
	if (NewComponent)
	{
		Pool->ActiveCascadeComponents.Add(NewComponent);
	}

	return NewComponent;
}

void UKREffectSubsystem::ReturnNiagaraComponentToPool(const FGameplayTag& EffectTag, UNiagaraComponent* Component)
{
	if (!Component)
	{
		return;
	}

	FKREffectPool* Pool = EffectPools.Find(EffectTag);
	if (!Pool)
	{
		Component->DestroyComponent();
		return;
	}

	Pool->ActiveNiagaraComponents.Remove(Component);
	Pool->AvailableNiagaraComponents.Add(Component);

	Component->Deactivate();
	Component->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

void UKREffectSubsystem::ReturnCascadeComponentToPool(const FGameplayTag& EffectTag, UParticleSystemComponent* Component)
{
	if (!Component)
	{
		return;
	}

	FKREffectPool* Pool = EffectPools.Find(EffectTag);
	if (!Pool)
	{
		Component->DestroyComponent();
		return;
	}

	Pool->ActiveCascadeComponents.Remove(Component);
	Pool->AvailableCascadeComponents.Add(Component);

	Component->Deactivate();
	Component->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

void UKREffectSubsystem::ReturnInstanceToPool(FKRActiveEffectInstance& Instance)
{
	if (Instance.EffectSystemType == EKREffectSystemType::Niagara)
	{
		ReturnNiagaraComponentToPool(Instance.EffectTag, Instance.NiagaraComponent);
	}
	else
	{
		ReturnCascadeComponentToPool(Instance.EffectTag, Instance.CascadeComponent);
	}
}

void UKREffectSubsystem::InitializePool(const UKREffectDefinition* EffectDef)
{
	if (!EffectDef || !EffectDef->PoolingSettings.bUsePooling)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FKREffectPool NewPool;
	NewPool.EffectTag = EffectDef->EffectTag;
	NewPool.EffectSystemType = EffectDef->EffectSystemType;
	NewPool.MaxPoolSize = EffectDef->PoolingSettings.MaxPoolSize;

	if (EffectDef->EffectSystemType == EKREffectSystemType::Niagara)
	{
		UNiagaraSystem* NiagaraSystem = EffectDef->NiagaraEffect.LoadSynchronous();
		if (!NiagaraSystem)
		{
			return;
		}

		for (int32 i = 0; i < EffectDef->PoolingSettings.PreallocatedCount; ++i)
		{
			UNiagaraComponent* Component = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				World,
				NiagaraSystem,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				FVector::OneVector,
				false,
				false,  // bAutoActivate
				ENCPoolMethod::None,
				true
			);
			if (Component)
			{
				NewPool.AvailableNiagaraComponents.Add(Component);
			}
		}
	}
	else
	{
		UParticleSystem* CascadeSystem = EffectDef->CascadeEffect.LoadSynchronous();
		if (!CascadeSystem)
		{
			return;
		}

		for (int32 i = 0; i < EffectDef->PoolingSettings.PreallocatedCount; ++i)
		{
			UParticleSystemComponent* Component = UGameplayStatics::SpawnEmitterAtLocation(
				World,
				CascadeSystem,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				FVector::OneVector,
				false,
				EPSCPoolMethod::None,
				false  // bAutoActivate
			);
			if (Component)
			{
				NewPool.AvailableCascadeComponents.Add(Component);
			}
		}
	}

	EffectPools.Add(EffectDef->EffectTag, NewPool);
}

void UKREffectSubsystem::CleanupInactiveEffects()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	float CurrentTime = World->GetTimeSeconds();

	for (int32 i = ActiveEffects.Num() - 1; i >= 0; --i)
	{
		FKRActiveEffectInstance& Instance = ActiveEffects[i];
		if (!Instance.IsValid() || !Instance.IsActive())
		{
			if (Instance.IsValid())
			{
				ReturnInstanceToPool(Instance);
			}
			ActiveEffects.RemoveAtSwap(i);
			continue;
		}

		if (Instance.ShouldDestroy(CurrentTime))
		{
			UFXSystemComponent* Component = Instance.GetFXComponent();
			if (Component)
			{
				Component->Deactivate();
			}
			ReturnInstanceToPool(Instance);
			ActiveEffects.RemoveAtSwap(i);
		}
	}
}

bool UKREffectSubsystem::CanSpawnEffect(const UKREffectDefinition* EffectDef, const FVector& Location) const
{
	if (!EffectDef)
	{
		return false;
	}
	
	int32 ActiveCount = 0;
	for (const FKRActiveEffectInstance& Instance : ActiveEffects)
	{
		if (Instance.EffectTag.MatchesTagExact(EffectDef->EffectTag) && Instance.IsValid())
		{
			++ActiveCount;
		}
	}

	if (ActiveCount >= EffectDef->PoolingSettings.MaxConcurrentInstances)
	{
		return false;
	}

	return true;
}

void UKREffectSubsystem::RemoveLowPriorityEffects(int32 NumToRemove)
{
	ActiveEffects.Sort([this](const FKRActiveEffectInstance& A, const FKRActiveEffectInstance& B)
	{
		return A.SpawnTime < B.SpawnTime;
	});

	for (int32 i = 0; i < NumToRemove && ActiveEffects.Num() > 0; ++i)
	{
		FKRActiveEffectInstance& Instance = ActiveEffects[0];
		if (Instance.IsValid())
		{
			UFXSystemComponent* Component = Instance.GetFXComponent();
			if (Component)
			{
				Component->Deactivate();
			}
			ReturnInstanceToPool(Instance);
		}
		ActiveEffects.RemoveAtSwap(0);
	}
}

void UKREffectSubsystem::ConfigureNiagaraComponent(UNiagaraComponent* Component, const UKREffectDefinition* EffectDef, const FTransform& Transform)
{
	if (!Component || !EffectDef)
	{
		return;
	}

	UNiagaraSystem* NiagaraSystem = EffectDef->NiagaraEffect.LoadSynchronous();
	if (NiagaraSystem)
	{
		Component->SetAsset(NiagaraSystem);
	}
	
	Component->SetWorldTransform(Transform);
	Component->SetRelativeScale3D(EffectDef->SpawnSettings.Scale);

	if (!Component->IsRegistered())
	{
		Component->RegisterComponent();
	}
}

void UKREffectSubsystem::ConfigureCascadeComponent(UParticleSystemComponent* Component, const UKREffectDefinition* EffectDef, const FTransform& Transform)
{
	if (!Component || !EffectDef)
	{
		return;
	}

	UParticleSystem* CascadeSystem = EffectDef->CascadeEffect.LoadSynchronous();
	if (CascadeSystem)
	{
		Component->SetTemplate(CascadeSystem);
	}

	Component->SetWorldTransform(Transform);
	Component->SetRelativeScale3D(EffectDef->SpawnSettings.Scale);

	if (!Component->IsRegistered())
	{
		Component->RegisterComponent();
	}
}