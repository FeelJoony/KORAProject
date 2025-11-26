#include "Weapons/KRWeaponInstance.h"

#include "Equipment/KREquipmentDefinition.h"
#include "Item/Weapons/KRWeaponBase.h"
#include "Kismet/KismetMathLibrary.h"

UKRWeaponInstance::UKRWeaponInstance(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    AttackPower = 0.0f;
    CritChance = 0.0f;
    CritMultiplier = 1.0f;
    Range = 100.0f;
    AttackSpeed = 1.0f;
    CurrentAmmo = 0;
    MaxAmmo = 0;
    ReloadTime = 0.0f;
}

void UKRWeaponInstance::OnEquipped(const TArray<FKREquipmentActorToSpawn>& ActorsToSpawn)
{
    SpawnEquipmentActors(ActorsToSpawn);

    // 장착 시 로직 (예: HUD 업데이트, 애니메이션 변경)
    UE_LOG(LogTemp, Log, TEXT("Weapon Equipped: %s"), *WeaponType.ToString());
}

void UKRWeaponInstance::OnUnequipped()
{
    Super::OnUnequipped();

    UE_LOG(LogTemp, Log, TEXT("Weapon Unequipped: %s"), *WeaponType.ToString());
}

void UKRWeaponInstance::InitializeStats(const UInventoryFragment_SetStats* StatsFragment)
{
    if (!StatsFragment) return;

    AttackPower = StatsFragment->GetStatByTag(FGameplayTag::RequestGameplayTag("Weapon.Stat.ATK"));
    CritChance = StatsFragment->GetStatByTag(FGameplayTag::RequestGameplayTag("Weapon.Stat.CritChance"));
    CritMultiplier = StatsFragment->GetStatByTag(FGameplayTag::RequestGameplayTag("Weapon.Stat.CritMulti"));
    Range = StatsFragment->GetStatByTag(FGameplayTag::RequestGameplayTag("Weapon.Stat.Range"));
    AttackSpeed = StatsFragment->GetStatByTag(FGameplayTag::RequestGameplayTag("Weapon.Stat.AttackSpeed"));

    MaxAmmo = StatsFragment->GetStatByTag(FGameplayTag::RequestGameplayTag("Weapon.Stat.Capacity"));
    CurrentAmmo = MaxAmmo;
    ReloadTime = StatsFragment->GetStatByTag(FGameplayTag::RequestGameplayTag("Weapon.Stat.ReloadTime"));
}

void UKRWeaponInstance::SpawnEquipmentActors(const TArray<FKREquipmentActorToSpawn>& ActorsToSpawn)
{
    AActor* Owner = GetTypedOuter<AActor>();
    if (!Owner) return;

    UWorld* World = Owner->GetWorld();
    if (!World) return;

    for (const auto& SpawnInfo : ActorsToSpawn)
    {
        TSubclassOf<AActor> ActorClass = SpawnInfo.ActorToSpawn;
        if (!ActorClass) continue;

        AKRWeaponBase* SpawnedWeapon = World->SpawnActor<AKRWeaponBase>(
            ActorClass,
            SpawnInfo.AttachTransform
        );

        if (SpawnedWeapon)
        {
            SpawnedWeaponActor = SpawnedWeapon;

            if (USkeletalMeshComponent* Mesh = Owner->FindComponentByClass<USkeletalMeshComponent>())
            {
                SpawnedWeapon->AttachToComponent(
                    Mesh,
                    FAttachmentTransformRules::SnapToTargetNotIncludingScale,
                    SpawnInfo.AttachSocket
                );
            }
        }
    }
}


float UKRWeaponInstance::CalculateDamage(bool& bOutIsCritical)
{
    float BaseDamage = AttackPower * EnhanceMultiplier;

    // 크리티컬 판정
    float RandomValue = UKismetMathLibrary::RandomFloatInRange(0.0f, 1.0f);
    bOutIsCritical = (RandomValue <= CritChance);

    if (bOutIsCritical)
    {
        BaseDamage *= CritMultiplier;
    }

    return BaseDamage;
}

void UKRWeaponInstance::ApplyEnhanceLevel(int32 EnhanceLevel)
{
    EnhanceMultiplier = 1.0f + (EnhanceLevel * EnhanceDamageMultiplierPerLevel);
    UE_LOG(LogTemp, Log, TEXT("Weapon Enhanced: Level %d, Multiplier %.2f"), EnhanceLevel, EnhanceMultiplier);
}