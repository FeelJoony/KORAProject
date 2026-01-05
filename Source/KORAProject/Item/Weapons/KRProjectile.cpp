#include "Item/Weapons/KRProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/AttributeSets/KRCombatCommonSet.h"
#include "GameplayTag/KRSetByCallerTag.h"

AKRProjectile::AKRProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->InitSphereRadius(15.0f);
	SphereComponent->SetCollisionProfileName(TEXT("Projectile"));
	SphereComponent->OnComponentHit.AddDynamic(this, &AKRProjectile::OnHit);
	RootComponent = SphereComponent;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = SphereComponent;
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->MaxSpeed = 3000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	InitialLifeSpan = LifeSpan;
}

void AKRProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (GetInstigator())
	{
		SphereComponent->IgnoreActorWhenMoving(GetInstigator(), true);
	}
}

void AKRProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this || !GetInstigator())
	{
		Destroy();
		return;
	}

	if (ImpactVFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetInstigator());

	if (SourceASC && TargetASC && DamageEffectClass)
	{
		bool bFound = false;
		float AttackPower = SourceASC->GetGameplayAttributeValue(UKRCombatCommonSet::GetAttackPowerAttribute(), bFound);
		
		if (!bFound) AttackPower = 0.f;
		
		FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
		Context.AddHitResult(Hit);
		
		FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, Context);
		
		if (SpecHandle.IsValid())
		{
			SpecHandle.Data->SetSetByCallerMagnitude(KRTAG_SETBYCALLER_BASEDAMAGE, AttackPower);
			SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
		}
	}

	Destroy();
}

