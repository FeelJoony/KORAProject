#include "Weapons/KRWeaponInstance.h"

#include "EnhancedInputSubsystems.h"
#include "KRWeaponAttributeSet.h"
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
#include "Characters/KRHeroCharacter.h"

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
    Super::OnEquipped(ActorsToSpawn);

    if (CachedIMC)
    {
        if (APawn* MyPawn = GetPawn())
        {
            if (APlayerController* PC = Cast<APlayerController>(MyPawn->GetController()))
            {
                if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
                {
                    Subsystem->AddMappingContext(CachedIMC, CachedIMCPriority);
                }
            }
        }
    }
    
    // 장착 시 로직 (예: HUD 업데이트, 애니메이션 변경)
    UE_LOG(LogTemp, Log, TEXT("Weapon Equipped: %s"), *WeaponType.ToString());
}

void UKRWeaponInstance::OnUnequipped()
{
    if (ACharacter* Char = Cast<ACharacter>(GetPawn()))
    {
        Char->GetMesh()->LinkAnimClassLayers(nullptr);
    }
    
    if (CachedIMC)
    {
        if (APawn* MyPawn = GetPawn())
        {
            if (APlayerController* PC = Cast<APlayerController>(MyPawn->GetController()))
            {
                if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
                {
                    Subsystem->RemoveMappingContext(CachedIMC);
                }
            }
        }
    }
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

        CachedWeaponActorClass = EquipFragment->WeaponActorToSpawn;
        CachedAttachSocket = EquipFragment->AttachSocketName;
        CachedAttachTransform = EquipFragment->AttachTransform;
        CachedIMC = EquipFragment->WeaponIMC;
        CachedIMCPriority = EquipFragment->InputPriority;
        CachedAbilitySets = EquipFragment->AbilitySetsToGrant;

        if (CachedWeaponActorClass)
        {
            UE_LOG(LogTemp, Warning, TEXT(">> Weapon Instance Initialized! ActorClass: %s"), *GetNameSafe(CachedWeaponActorClass));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT(">> CachedWeaponActorClass is NULL! Fragment data is empty?"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[EquipTest] 3. Instance could not find Fragment!"));
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

void UKRWeaponInstance::SpawnEquipmentActors(const TArray<FKREquipmentActorToSpawn>& ActorsToSpawn)
{
    Super::SpawnEquipmentActors(ActorsToSpawn);
    UE_LOG(LogTemp, Warning, TEXT(">> SpawnEquipmentActors Called. CachedClass: %s"), *GetNameSafe(CachedWeaponActorClass));
    if (CachedWeaponActorClass)
    {
        AActor* OwnerActor = GetPawn();
        UWorld* World = GetWorld();

        if (World && OwnerActor)
        {
            bool bShouldHideOnSpawn = false;

            if (OwnerActor->IsA(AKRHeroCharacter::StaticClass()))
            {
                bShouldHideOnSpawn = true;
            }
            
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = OwnerActor;
            SpawnParams.Instigator = Cast<APawn>(OwnerActor);
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

            AActor* NewActor = World->SpawnActorDeferred<AActor>(CachedWeaponActorClass, FTransform::Identity, OwnerActor, Cast<APawn>(OwnerActor), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

            if (NewActor)
            {
                UE_LOG(LogTemp, Warning, TEXT(">> Actor Spawned SUCCESS: %s"), *GetNameSafe(NewActor));
                
                NewActor->FinishSpawning(FTransform::Identity, true);
                
                if (ACharacter* Char = Cast<ACharacter>(OwnerActor))
                {
                    FName SocketName = CachedAttachSocket.IsNone() ? FName("WeaponSocket") : CachedAttachSocket;
                    NewActor->AttachToComponent(Char->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
                    NewActor->SetActorRelativeTransform(CachedAttachTransform);
                }

                if (AKRWeaponBase* WeaponBase = Cast<AKRWeaponBase>(NewActor))
                {
                    WeaponBase->SetOwningInstance(this);
                    if (bShouldHideOnSpawn)
                    {
                        WeaponBase->SetWeaponVisibility(false);
                    }
                    else
                    {
                        WeaponBase->SetWeaponVisibility(true);
                    }
                }
                
                SpawnedWeaponActor = NewActor;
                SpawnedActors.Add(NewActor);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT(">> Actor Spawn FAILED! Check Collision or World context."));
            }
        }
    }
}

void UKRWeaponInstance::DestroyEquipmentActors()
{
    if (SpawnedWeaponActor)
    {
        SpawnedWeaponActor->Destroy();
        SpawnedWeaponActor = nullptr;
    }
    Super::DestroyEquipmentActors();
}

void UKRWeaponInstance::SetWeaponActiveState(bool bIsActive)
{
    if (AKRWeaponBase* WeaponBase = Cast<AKRWeaponBase>(SpawnedWeaponActor))
    {
        WeaponBase->SetWeaponVisibility(bIsActive);
    }
}

void UKRWeaponInstance::GrantWeaponAbilities(UKRAbilitySystemComponent* ASC)
{
    if (!ASC) return;

    RemoveWeaponAbilities(ASC);

    for (const UKRAbilitySet* AbilitySet : CachedAbilitySets)
    {
        if (AbilitySet)
        {
            AbilitySet->GiveToAbilitySystem(ASC, &GrantedHandles, this);
        }
    }

    if (UKRWeaponAttributeSet* WeaponSet = const_cast<UKRWeaponAttributeSet*>(ASC->GetSet<UKRWeaponAttributeSet>()))
    {
        WeaponSet->SetWeaponAttackPower(AttackPower);
        WeaponSet->SetCriticalChance(CritChance);
        WeaponSet->SetCriticalMultiplier(CritMultiplier);
        WeaponSet->SetAttackSpeed(AttackSpeed);
    }
    
}

void UKRWeaponInstance::RemoveWeaponAbilities(UKRAbilitySystemComponent* ASC)
{
    if (!ASC) return;
    GrantedHandles.TakeFromAbilitySystem(ASC);
}
