#include "GAS/Abilities/HeroAbilities/KRGA_Scanning.h"

#include "ScanFunctionLibrary.h"
#include "ScannerController.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"

void UKRGA_Scanning::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	CachedCharacter = Cast<ACharacter>(ActorInfo->AvatarActor);
	StartScan();
	GetWorld()->GetTimerManager().SetTimer(ScanTimer,this,&UKRGA_Scanning::SpawnOverlapSphere,0.1f,true);
}

void UKRGA_Scanning::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_Scanning::StartScan()
{
	CachedScannerController = UScanFunctionLibrary::GetSphereRevealController(GetWorld());
	if (CachedScannerController)
	{
		CachedScannerController->StartScan();
	}
}

void UKRGA_Scanning::SpawnOverlapSphere()
{
	OverlapActors.Empty();
	
	float Radius=CachedScannerController->GetRadius();
	
	if (CachedScannerController->SpawnedFX->IsHidden())
	{
		OnEnded();
		return;
	}
	
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	//ObjectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel4);

	TArray<FOverlapResult> OutOverlaps;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetAvatarActorFromActorInfo());

	if (!CachedCharacter)
	{
		OnEnded();
		return;
	}

	bool bHit = GetWorld()->OverlapMultiByObjectType(
		OutOverlaps,
		CachedCharacter->GetActorLocation(),
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(Radius),
		Params
	);

	DrawDebugSphere(
		GetWorld(),
		CachedCharacter->GetActorLocation(),
		Radius,
		10,
		FColor::Red,
		false,
		0.1,
		0
	);

	//UE_LOG(LogTemp,Warning,TEXT("Scan : %f"), Radius);
	
	if (bHit)
	{
		for (FOverlapResult& Overlap : OutOverlaps)
		{
			if (AActor* Act = Cast<AActor>(Overlap.GetActor()))
			{
				OverlapActors.Add(Act);
				//UE_LOG(LogTemp,Warning,TEXT("[UKRGA_Scanning] OverlapActors : %s"),*Act->GetName());
			}
		}
	}
	ApplyOutliner();
}

void UKRGA_Scanning::OnEnded()
{
	GetWorld()->GetTimerManager().ClearTimer(ScanTimer);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}