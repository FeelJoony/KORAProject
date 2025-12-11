#include "Interaction/GrappleVolume.h"

#include "CollisionDebugDrawingPublic.h"
#include "KismetTraceUtils.h"
#include "Camera/CameraComponent.h"
#include "Characters/KRHeroCharacter.h"
#include "Components/BoxComponent.h"

AGrappleVolume::AGrappleVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxComponent->SetupAttachment(RootComponent);
	
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComponent->SetCollisionObjectType(ECC_WorldDynamic);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	BoxComponent->OnComponentBeginOverlap.AddDynamic(
		this, &AGrappleVolume::OnOverlapBegin
	);
	BoxComponent->OnComponentEndOverlap.AddDynamic(
		this, &AGrappleVolume::OnOverlapEnd
	);
}

void AGrappleVolume::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult & SweepResult)
{
	CachedPlayer = Cast<AKRHeroCharacter>(OtherActor);
	
	if (CachedPlayer)
	{
		CachedCamera = CachedPlayer->GetComponentByClass<UCameraComponent>();
		if (!CachedCamera) return;
		
		for (AActor* GrapplePoint : GrapplePoints)
		{
			GrapplePoint->SetActorHiddenInGame(false);
			UE_LOG(LogTemp, Warning, TEXT("Overlaped Name : %s"),*GrapplePoint->GetName());
		}

		GetWorld()->GetTimerManager().SetTimer(Timer,this, &AGrappleVolume::WidgetBehavior,0.1f,true);
		
		IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(CachedPlayer);
		if (!ASI) return;

		//for문으로 BP_VisibleUI();
		//시야에 있고 가장 가까운 것 찾아서 있으면 ASC에 "State.Can.Grapple" 추가하고

		
		//멤버 변수에 위치 저장
		
		
		UE_LOG(LogTemp, Warning, TEXT("Player Is Overlaped"));
	}
}

void AGrappleVolume::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (AKRHeroCharacter* PlayerCharacter = Cast<AKRHeroCharacter>(OtherActor))
	{
		CachedPlayer = nullptr;
		CachedCamera = nullptr;
		
		UE_LOG(LogTemp, Warning, TEXT("Overlap Out"));
	}
	
	for (AActor* GrapplePoint : GrapplePoints)
	{
		GrapplePoint->SetActorHiddenInGame(true);
	}
	
	CachedGrapplePoint = nullptr;
	
	GetWorld()->GetTimerManager().ClearTimer(Timer);
}

void AGrappleVolume::WidgetBehavior()
{
	if (!CachedPlayer || !CachedCamera) return;//return대신
	
	FVector StartLocation = CachedCamera->GetComponentLocation();
	FRotator StartRotation = CachedCamera->GetComponentRotation();

	LineTrace(StartLocation, StartRotation);
	
	for (AActor* GrapplePoint : GrapplePoints)
	{
		FVector GrapplePointLocation = GrapplePoint->GetActorLocation();
		FVector Direction = (StartLocation - GrapplePointLocation).GetSafeNormal();
		GrapplePoint->SetActorRotation(Direction.Rotation());
		
		//UE_LOG(LogTemp, Warning, TEXT("GrapplePoint Direction : %f, %f, %f"),Direction.X,Direction.Y,Direction.Z);
	}
}

void AGrappleVolume::LineTrace(const FVector& StartLocation, const FRotator& StartRotation)
{
	FHitResult HitResult;
    FCollisionQueryParams Params;
	FVector EndLocation = StartRotation.Vector() * TraceRange + StartLocation;
	GetWorld()->LineTraceSingleByChannel(HitResult,StartLocation,EndLocation,ECC_Visibility,Params);
	AActor* GrapplePointActor = HitResult.GetActor();
	if (GrapplePointActor && GrapplePoints.Contains(GrapplePointActor))
	{
		ChangeWidgetClass(GrapplePointActor);
		UE_LOG(LogTemp, Warning, TEXT("GrapplePoint HitResult : %s"),*HitResult.GetActor()->GetName());
	}
}
