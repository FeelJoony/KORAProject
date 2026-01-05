// Copyright (c) 2025 Studio3F. All Rights Reserved.

#include "ScannerController.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "GameFramework/Character.h"

#include "Engine/World.h"                
#include "Engine/EngineTypes.h"          
#include "Components/StaticMeshComponent.h" 
#include "Engine/StaticMesh.h" 

AScannerController::AScannerController()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AScannerController::BeginPlay()
{
    Super::BeginPlay();

    if (!GetMPCI())
    {
        UE_LOG(LogTemp, Warning, TEXT("[SphereReveal] MPCI null on %s"), *GetName());
        return;
    }

    Radius = StartRadius;
    SetFloat(ParamEditorOverride, 0.f);
    SetFloat(ParamActive, 0.f);
    SetFloat(ParamRadius, Radius);

    CachedCenter = IsValid(FollowActor) ? FollowActor->GetActorLocation() : GetActorLocation();
    SetVec(ParamCenter, CachedCenter);

    if (bAutoSpawnFX && ScanFXClass)
    {
        FActorSpawnParameters P;
        P.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        SpawnedFX = GetWorld()->SpawnActor<AActor>(ScanFXClass, FTransform(CachedCenter));
        if (SpawnedFX) SpawnedFX->SetActorHiddenInGame(true); // Deactivate, at first.
    }

    if (SpawnedFX)
    {
        if (UStaticMeshComponent* MeshComp = SpawnedFX->FindComponentByClass<UStaticMeshComponent>())
        {
            FXMeshRadius = MeshComp->GetStaticMesh()->GetBounds().SphereRadius;
        }
    }
}

void AScannerController::Tick(float dt)
{
    Super::Tick(dt);

    // cool down countdown
    if (!bHoldingAtMax && GetFloat(ParamActive) < 0.5f && bCoolingDown)
    {
        CooldownTimer -= dt;
        if (CooldownTimer <= 0.f)
        {
            bCoolingDown = false;
            CooldownTimer = 0.f;
        }
    }

    FVector NewCenter;
    if (IsValid(FollowActor))
    {
        NewCenter=FollowActor->GetActorLocation();
    }
    else
    {
         ReCacheFollowActor();
    }

    
    if (!NewCenter.Equals(CachedCenter, 0.1f))
    {
        CachedCenter = NewCenter;
        SetVec(ParamCenter, CachedCenter);
    }

    const bool bActive = GetFloat(ParamActive) > 0.5f;

    if (bActive)
    {
        // Max 도달 전에는 확장
        if (!bHoldingAtMax && ExpandSpeed > 0.f)
        {
            Radius += ExpandSpeed * dt;
            SetFloat(ParamRadius, Radius);

            const float EffectiveMax = (MaxRadius > 0.f) ? MaxRadius : MaxRadiusFallback;
            if (EffectiveMax > 0.f && Radius >= EffectiveMax)
            {
                // if touch the max, start clamp + hold
                Radius = EffectiveMax;
                SetFloat(ParamRadius, Radius);

                bHoldingAtMax = true;
                HoldTimer = HoldAtMaxSeconds;
            }
        }

        // hold timer
        if (bHoldingAtMax)
        {
            HoldTimer -= dt;
            if (HoldTimer <= 0.f)
            {
                // after holding, End HardCut
                StopScan(true);   
                BeginCooldown(); 
                bHoldingAtMax = false;
            }
        }
    }

    UpdateFX(bActive);
}

void AScannerController::UpdateFX(bool bActive)
{
    if (!SpawnedFX) return;

    SpawnedFX->SetActorHiddenInGame(!bActive);
    if (!bActive) return;

    SpawnedFX->SetActorLocation(CachedCenter);

    const float Scale = (FXMeshRadius > 0.f) ? (Radius / FXMeshRadius) : 1.f;
    SpawnedFX->SetActorScale3D(FVector(Scale));
}


void AScannerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    SetFloat(ParamActive, 0.f);
    SetFloat(ParamRadius, 0.f);
    
    Super::EndPlay(EndPlayReason);
}

void AScannerController::ReCacheFollowActor()
{
    if (IsFollowPlayer0 && GetWorld())
    {
        FollowActor=UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
        return;
    }
    //UE_LOG(LogTemp,Error, TEXT("[SphereReveal] FollowActor null"));
}

UMaterialParameterCollectionInstance* AScannerController::GetMPCI() const
{
    if (!GetWorld() || !ScannerMPC) return nullptr;

    UMaterialParameterCollectionInstance* MPCI = GetWorld()->GetParameterCollectionInstance(ScannerMPC);

    return IsValid(MPCI) ? MPCI : nullptr;
}

void AScannerController::StartScan()
{
    //if (!MPCI) return;

    if (bCoolingDown) return;

    // Prevent restart if already active or holding at max
    if (GetFloat(ParamActive) > 0.5f || bHoldingAtMax) return;

    if (MaxRadius > 0.f && Radius > MaxRadius)
    {
        Radius = MaxRadius;
    }

    SetFloat(ParamRadius, Radius);
    SetFloat(ParamActive, 1.f);

    // Reset hold state
    bHoldingAtMax = false;
    HoldTimer = 0.f;

    if (SpawnedFX) { SpawnedFX->SetActorHiddenInGame(false); UpdateFX(true); }

    if (bEnableSFX && SFX_Start)
    {
        UGameplayStatics::PlaySoundAtLocation(this, SFX_Start, CachedCenter, SFXVolume);
    }
}

void AScannerController::StopScan(bool bHardCut)
{
    // Deactivate flag
    SetFloat(ParamActive, 0.f);

    // If hard cut, reset radius immediately to zero
    if (bHardCut)
    {
        Radius = 0.f;
        SetFloat(ParamRadius, 0.f);
    }
    
    if (SpawnedFX) { SpawnedFX->SetActorHiddenInGame(true); }

    // Clear hold state
    bHoldingAtMax = false;
    HoldTimer = 0.f;

    // Start Cooldown (also applies when stopped manually)
    BeginCooldown();
}

void AScannerController::BeginCooldown()
{
    bCoolingDown = (CooldownSeconds > 0.f);
    CooldownTimer = bCoolingDown ? CooldownSeconds : 0.f;
}

void AScannerController::SetVec(FName N, const FVector& V)
{
    //if (MPCI) MPCI->SetVectorParameterValue(N, FLinearColor(V));
    if (UMaterialParameterCollectionInstance* MPCI = GetMPCI())
    {
        MPCI->SetVectorParameterValue(N, FLinearColor(V));
    }
}
void AScannerController::SetFloat(FName N, float V)
{
    //if (MPCI) MPCI->SetScalarParameterValue(N, V);
    if (UMaterialParameterCollectionInstance* MPCI = GetMPCI())
    {
        MPCI->SetScalarParameterValue(N, V);
    }
}
float AScannerController::GetFloat(FName N) const
{
    float Out = 0.f;
    //if (MPCI) MPCI->GetScalarParameterValue(N, Out);
    if (UMaterialParameterCollectionInstance* MPCI = GetMPCI())
    {
        MPCI->GetScalarParameterValue(N, Out);
    }
    return Out;
}
