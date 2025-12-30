#include "LostCurrencyDrop.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/KRCurrencyComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameplayTag/KRShopTag.h"

ALostCurrencyDrop::ALostCurrencyDrop()
{
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	Collision->SetSphereRadius(80.f);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	Collision->OnComponentBeginOverlap.AddDynamic(this, &ALostCurrencyDrop::OnOverlapBegin);
}

void ALostCurrencyDrop::BeginPlay()
{
	Super::BeginPlay();

	// 여기서 파티클, 회전, 사운드 재생 등 연출 처리하면 됨
}

void ALostCurrencyDrop::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (APawn* Pawn = Cast<APawn>(OtherActor))
	{
		if (APlayerState* PS = Pawn->GetPlayerState())
		{
			if (UKRCurrencyComponent* Currency = PS->FindComponentByClass<UKRCurrencyComponent>())
			{
				UE_LOG(LogTemp, Warning, TEXT("Currency : %d"), Currency->GetCurrency(KRTAG_CURRENCY_PURCHASE_GEARING));
				UE_LOG(LogTemp, Warning, TEXT("Lost Currency : %d"), Currency->GetLostCurrency(KRTAG_CURRENCY_PURCHASE_GEARING));
				
				Currency->ReclaimLostCurrency();
				bCollected = true;
				Destroy();
			}
		}
	}

}
