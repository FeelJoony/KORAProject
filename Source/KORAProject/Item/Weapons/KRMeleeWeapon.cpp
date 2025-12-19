#include "Item/Weapons/KRMeleeWeapon.h"
#include "Components/BoxComponent.h"
#include "Components/KRCombatComponent.h"
#include "Equipment/KREquipmentInstance.h"
#include "Equipment/KREquipmentDefinition.h"
#include "Inventory/Fragment/InventoryFragment_EquippableItem.h"
#include "Inventory/Fragment/InventoryFragment_SetStats.h"

AKRMeleeWeapon::AKRMeleeWeapon()
{
	WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollisionBox"));
	WeaponCollisionBox->SetupAttachment(GetRootComponent());
	WeaponCollisionBox->SetBoxExtent(FVector(20.f));
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// 델리게이트 바인딩은 BeginPlay에서 수행 (CDO 문제 방지)
	WeaponCollisionBox->SetGenerateOverlapEvents(false);
}

void AKRMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();

	// BeginPlay에서 델리게이트 바인딩 (AddUniqueDynamic으로 안전성 확보)
	WeaponCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &AKRMeleeWeapon::OnCollisionBoxBeginOverlap);
	WeaponCollisionBox->OnComponentEndOverlap.AddUniqueDynamic(this, &AKRMeleeWeapon::OnCollisionBoxEndOverlap);
}

void AKRMeleeWeapon::ConfigureWeapon(UKRInventoryItemInstance* InInstance)
{
	Super::ConfigureWeapon(InInstance);

	// 근접 무기 특화 설정이 필요하면 여기서(예 : 히트 박스 크기 조절 등)
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
