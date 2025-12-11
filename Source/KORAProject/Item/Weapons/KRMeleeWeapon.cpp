#include "Item/Weapons/KRMeleeWeapon.h"
#include "Components/BoxComponent.h"
#include "Components/KRCombatComponent.h"

AKRMeleeWeapon::AKRMeleeWeapon()
{
	WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollisionBox"));
	WeaponCollisionBox->SetupAttachment(GetRootComponent());
	WeaponCollisionBox->SetBoxExtent(FVector(20.f));
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBoxBeginOverlap);
	WeaponCollisionBox->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBoxEndOverlap);
	WeaponCollisionBox->SetGenerateOverlapEvents(false);
}

void AKRMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();

	//WeaponCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AKRMeleeWeapon::OnCollisionBoxBeginOverlap);
}

void AKRMeleeWeapon::OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AActor* WeaponOwner = GetOwner();
	if (!WeaponOwner || !OtherActor || OtherActor == WeaponOwner) return;

	if (UActorComponent* Comp = WeaponOwner->GetComponentByClass(UKRCombatComponent::StaticClass()))
	{
		if (UKRCombatComponent* CombatComp = Cast<UKRCombatComponent>(Comp))
		{
			CombatComp->HandleMeleeHit(OtherActor, SweepResult);
		}
	}
	APawn* WeaponOwningPawn = GetInstigator<APawn>();

	check(WeaponOwningPawn);

	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (WeaponOwningPawn != HitPawn)
		{
			//ReportHit(OtherActor, SweepResult);
		}

		// TODO : Implement hit check for enemy
	}
}

void AKRMeleeWeapon::OnCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APawn* WeaponOwningPawn = GetInstigator<APawn>();

	check(WeaponOwningPawn);

	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (WeaponOwningPawn != HitPawn)
		{
			FHitResult Dummy;
			// ReportPulled(OtherActor, Dummy);
		}

		// TODO : Implement hit check for enemy
	}
}

void AKRMeleeWeapon::SetHitCollisionEnabled(bool bEnabled)
{
	WeaponCollisionBox->SetCollisionEnabled(bEnabled ? ECollisionEnabled::QueryOnly
										 : ECollisionEnabled::NoCollision);
	WeaponCollisionBox->SetGenerateOverlapEvents(bEnabled);
}
