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
}

void UKRGA_Scanning::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	for (TPair<AActor*, bool>& Pair : AllOverlapActors)
	{
		UE_LOG(LogTemp, Log, TEXT("[AndAbility] Actor Name : %s"), *Pair.Key->GetName());
		ClearOutliner(Pair.Key); //ClearOverlap() 호출하기
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_Scanning::StartScan()
{
	CachedScannerController = UScanFunctionLibrary::GetSphereRevealController(GetWorld());
	if (CachedScannerController)
	{
		CachedScannerController->StartScan();
		GetWorld()->GetTimerManager().SetTimer(ScanTimer,this,&UKRGA_Scanning::SpawnOverlapSphere,0.1f,true);//Task로 변경
	}
}

void UKRGA_Scanning::SpawnOverlapSphere()
{
	CurrentOverlapActors.Empty();
	
	if (!CachedCharacter)
	{
		OnEnded();
		return;
	}
	
	if (CachedScannerController->SpawnedFX->IsHidden())
	{
		OnEnded();
		return;
	}
	
	TArray<FOverlapResult> OutOverlaps;
	
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	
	float Radius=CachedScannerController->GetRadius();
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetAvatarActorFromActorInfo());
	
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
	
	//Overlap된 액터 가져오기
	if (bHit)
	{
		for (FOverlapResult& Overlap : OutOverlaps)
		{
			if (AActor* Act = Cast<AActor>(Overlap.GetActor()))
			{
				CurrentOverlapActors.Add(Act);
				AllOverlapActors.Add(Act, true);
				UE_LOG(LogTemp, Log, TEXT("[AndAbility] Actor Name : %s"), *Act->GetName());
			}
		}
	}
	
	//Outline 삭제
	for (auto It = AllOverlapActors.CreateIterator(); It; ++It)
	{
		if (It.Value() == true)
		{
			ApplyOutliner(It.Key());
		}
		else
		{
			ClearOutliner(It.Key());
			It.RemoveCurrent();
		}
	}
	
	//현재 배열 전부 false로 변경 
	for (TPair<AActor*, bool>& Pair : AllOverlapActors)
	{
		Pair.Value = false;
	}
}

void UKRGA_Scanning::OnEnded()
{
	GetWorld()->GetTimerManager().ClearTimer(ScanTimer);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}