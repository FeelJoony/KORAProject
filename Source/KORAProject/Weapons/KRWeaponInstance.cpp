#include "Weapons/KRWeaponInstance.h"
#include "Equipment/KREquipmentDefinition.h"
#include "Item/Weapons/KRWeaponBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "Inventory/KRInventoryItemInstance.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "Inventory/Fragment/InventoryFragment_SetStats.h"
#include "Inventory/Fragment/InventoryFragment_EnhanceableItem.h"
#include "SubSystem/KRDataTablesSubsystem.h"
#include "Data/CacheDataTable.h"
#include "Inventory/Fragment/InventoryFragment_EquippableItem.h"

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
    //SpawnEquipmentActors(ActorsToSpawn);

    // 장착 시 로직 (예: HUD 업데이트, 애니메이션 변경)
    UE_LOG(LogTemp, Log, TEXT("Weapon Equipped: %s"), *WeaponType.ToString());
}

void UKRWeaponInstance::OnUnequipped()
{
    Super::OnUnequipped();

    UE_LOG(LogTemp, Log, TEXT("Weapon Unequipped: %s"), *WeaponType.ToString());
}

void UKRWeaponInstance::InitializeFromItem(UKRInventoryItemInstance* ItemInstance)
{
    if (!ItemInstance)
    {
        return;
    }

    const UKRInventoryItemDefinition* ItemDef = ItemInstance->GetItemDef();
    if (!ItemDef)
    {
        return;
    }

    // Stats Fragment
    if (const UInventoryFragment_SetStats* StatsFragment =
        Cast<UInventoryFragment_SetStats>(ItemDef->FindFragmentByTag(
            UInventoryFragment_SetStats::GetStaticFragmentTag())))
    {
        InitializeStats(StatsFragment);
    }

    int32 EnhanceLevel = 0;
    if (const UInventoryFragment_EnhanceableItem* EnhanceFragment =
        Cast<UInventoryFragment_EnhanceableItem>(ItemDef->FindFragmentByTag(
            UInventoryFragment_EnhanceableItem::GetStaticFragmentTag())))
    {
        EnhanceLevel = EnhanceFragment->EnhanceLevel;
        WeaponType = EnhanceFragment->WeaponTypeTag;
    }

    ApplyEnhanceLevel(EnhanceLevel);

    if (const UInventoryFragment_EquippableItem* EquipFragment = Cast<UInventoryFragment_EquippableItem>(ItemDef->FindFragmentByTag(FGameplayTag::RequestGameplayTag("Fragment.Item.Equippable"))))
    {
        if (EquipFragment->EquippableAnimLayer)
        {
            if (APawn* MyPawn = GetPawn())
            {
                if (ACharacter* MyChar = Cast<ACharacter>(MyPawn))
                {
                    MyChar->GetMesh()->LinkAnimClassLayers(EquipFragment->EquippableAnimLayer);
                }
            }
        }
    }
}

void UKRWeaponInstance::InitializeStats(const UInventoryFragment_SetStats* StatsFragment)
{
    if (!StatsFragment) return;

    BaseAttackPower = StatsFragment->GetStatByTag(FGameplayTag::RequestGameplayTag("Weapon.Stat.ATK"));
    BaseCritChance = StatsFragment->GetStatByTag(FGameplayTag::RequestGameplayTag("Weapon.Stat.CritChance"));
    CritMultiplier = StatsFragment->GetStatByTag(FGameplayTag::RequestGameplayTag("Weapon.Stat.CritMulti"));
    Range = StatsFragment->GetStatByTag(FGameplayTag::RequestGameplayTag("Weapon.Stat.Range"));
    AttackSpeed = StatsFragment->GetStatByTag(FGameplayTag::RequestGameplayTag("Weapon.Stat.AttackSpeed"));

    MaxAmmo = StatsFragment->GetStatByTag(FGameplayTag::RequestGameplayTag("Weapon.Stat.Capacity"));
    CurrentAmmo = MaxAmmo;
    ReloadTime = StatsFragment->GetStatByTag(FGameplayTag::RequestGameplayTag("Weapon.Stat.ReloadTime"));

    AttackPower = BaseAttackPower;
    CritChance = BaseCritChance;

    UE_LOG(LogTemp, Warning, TEXT("[InstanceTest] ATK=%.1f CritChance=%.2f CritMulti=%.1f Range=%.1f Ammo=%d/%d Reload=%.2f"),
    AttackPower,
    CritChance,
    CritMultiplier,
    Range,
    CurrentAmmo,
    MaxAmmo,
    ReloadTime
);

}

// void UKRWeaponInstance::SpawnEquipmentActors(const TArray<FKREquipmentActorToSpawn>& ActorsToSpawn)
// {
//     AActor* Owner = GetTypedOuter<AActor>();
//     if (!Owner) return;
//
//     UWorld* World = Owner->GetWorld();
//     if (!World) return;
//
//     for (const auto& SpawnInfo : ActorsToSpawn)
//     {
//         TSubclassOf<AActor> ActorClass = SpawnInfo.ActorToSpawn;
//         if (!ActorClass) continue;
//
//         AKRWeaponBase* SpawnedWeapon = World->SpawnActor<AKRWeaponBase>(
//             ActorClass,
//             SpawnInfo.AttachTransform
//         );
//
//         if (SpawnedWeapon)
//         {
//             SpawnedWeaponActor = SpawnedWeapon;
//
//             if (USkeletalMeshComponent* Mesh = Owner->FindComponentByClass<USkeletalMeshComponent>())
//             {
//                 SpawnedWeapon->AttachToComponent(
//                     Mesh,
//                     FAttachmentTransformRules::SnapToTargetNotIncludingScale,
//                     SpawnInfo.AttachSocket
//                 );
//             }
//         }
//     }
// }


float UKRWeaponInstance::CalculateDamage(bool& bOutIsCritical)
{
    float BaseDamage = AttackPower;
    const float RandomValue = UKismetMathLibrary::RandomFloatInRange(0.0f, 1.0f);
    bOutIsCritical = (RandomValue <= CritChance);

    if (bOutIsCritical)
    {
        BaseDamage *= CritMultiplier;
    }

    return BaseDamage;
}

void UKRWeaponInstance::ApplyEnhanceLevel(int32 EnhanceLevel)
{
    CurrentEnhanceLevel = FMath::Max(0, EnhanceLevel);

    AttackPower = BaseAttackPower;
    CritChance = BaseCritChance;

    if (CurrentEnhanceLevel <= 0) return;

    AActor* Owner = GetTypedOuter<AActor>();
    if (!Owner) return;
    UWorld* World = Owner->GetWorld();
    if (!World) return;
    UGameInstance* GameInstance = World->GetGameInstance();
    if (!GameInstance) return;
    UKRDataTablesSubsystem* DTSubsystem = GameInstance->GetSubsystem<UKRDataTablesSubsystem>();
    if (!DTSubsystem) return;
    UCacheDataTable* CachedTable = DTSubsystem->GetTable(EGameDataType::WeaponEnhanceData);
    if (!CachedTable) return;
    UDataTable* DataTable = CachedTable->GetTable(); // Get WeaponEnhanceData
    if (!DataTable) return;
    
    // Enhance Data Table에서 무기 타입을 TypeTag로 넣어서 GetKey로 만들어서 그걸 기반으로 AddATK랑 AddCritChance를 만들 수 있도록 수정
    
}