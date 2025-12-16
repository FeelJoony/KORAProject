#include "Weapons/KRWeaponInstance.h"
/*
#include "EnhancedInputSubsystems.h"
#include "KRWeaponAttributeSet.h"
#include "Equipment/KREquipmentDefinition.h"
#include "Item/Weapons/KRWeaponBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Inventory/KRInventoryItemInstance.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "Inventory/Fragment/InventoryFragment_SetStats.h"
#include "Inventory/Fragment/InventoryFragment_EnhanceableItem.h"
#include "SubSystem/KRDataTablesSubsystem.h"
#include "Data/CacheDataTable.h"
#include "Inventory/Fragment/InventoryFragment_EquippableItem.h"
#include "Characters/KRHeroCharacter.h"
#include "Player/KRPlayerController.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "GameFramework/GameplayMessageSubsystem.h"

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

}

void UKRWeaponInstance::OnUnequipped()
{
    Super::OnUnequipped();
}

void UKRWeaponInstance::ActivateWeapon(AKRBaseCharacter* Character, AKRPlayerController* PC, UKRAbilitySystemComponent* ASC, EWeaponMessageAction Action, bool bBroadcast)
{
    GrantWeaponAbilities(ASC);
    ApplyWeaponAnimLayer(Character);
    AddWeaponInputContext(PC);

    if (AKRWeaponBase* WeaponBase = Cast<AKRWeaponBase>(SpawnedWeaponActor))
    {
        WeaponBase->SetWeaponVisibility(true);
        WeaponBase->PlayEquipEffect();
    }

    if (bBroadcast) BroadcastWeaponMessage(Action);
}

void UKRWeaponInstance::DeactivateWeapon(AKRBaseCharacter* Character, AKRPlayerController* PC, UKRAbilitySystemComponent* ASC, EWeaponMessageAction Action, bool bBroadcast)
{
    RemoveWeaponAbilities(ASC);
    RemoveWeaponInputContext(PC);
    RemoveWeaponAnimLayer(Character);

    if (AKRWeaponBase* WeaponBase = Cast<AKRWeaponBase>(SpawnedWeaponActor))
    {
        WeaponBase->PlayUnequipEffect();
    }

    if (bBroadcast) BroadcastWeaponMessage(Action);
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
        CachedAnimLayer = EquipFragment->EquippableAnimLayer;
        CachedWeaponActorClass = EquipFragment->WeaponActorToSpawn;
        CachedAttachSocket = EquipFragment->AttachSocketName;
        CachedAttachTransform = EquipFragment->AttachTransform;
        CachedIMC = EquipFragment->WeaponIMC;
        CachedIMCPriority = EquipFragment->InputPriority;
        CachedAbilities = EquipFragment->GrantedAbilities;
        LightAttackMontages = EquipFragment->LightAttackMontages;
        ChargeAttackMontages = EquipFragment->ChargeAttackMontages;
        ChargeAttackMontages = EquipFragment->ChargeAttackMontages;
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

    return 20.f;
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
                NewActor->FinishSpawning(FTransform::Identity, true);
                
                if (ACharacter* Char = Cast<ACharacter>(OwnerActor))
                {
                    FName SocketName = CachedAttachSocket.IsNone() ? FName("WeaponSocket") : CachedAttachSocket;
                    NewActor->AttachToComponent(Char->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
                    NewActor->SetActorRelativeTransform(CachedAttachTransform);
                }

                if (AKRWeaponBase* WeaponBase = Cast<AKRWeaponBase>(NewActor))
                {
                    WeaponBase->SetWeaponInstance(this);
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

void UKRWeaponInstance::ApplyWeaponAnimLayer(AKRBaseCharacter* TargetCharacter)
{
    if (TargetCharacter && CachedAnimLayer)
    {
        TargetCharacter->GetMesh()->LinkAnimClassLayers(CachedAnimLayer);
    }
}

void UKRWeaponInstance::RemoveWeaponAnimLayer(AKRBaseCharacter* TargetCharacter)
{
    if (TargetCharacter && CachedAnimLayer)
    {
        TargetCharacter->GetMesh()->UnlinkAnimClassLayers(CachedAnimLayer);
    }
}

void UKRWeaponInstance::AddWeaponInputContext(AKRPlayerController* TargetPC)
{
    if (!TargetPC || !CachedIMC) return;

    if (const ULocalPlayer* LocalPlayer = TargetPC->GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            Subsystem->RemoveMappingContext(CachedIMC);
            Subsystem->AddMappingContext(CachedIMC, CachedIMCPriority);
        }
    }
}

void UKRWeaponInstance::RemoveWeaponInputContext(AKRPlayerController* TargetPC)
{
    if (!TargetPC || !CachedIMC) return;

    if (const ULocalPlayer* LocalPlayer = TargetPC->GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            Subsystem->RemoveMappingContext(CachedIMC);
        }
    }
}

UAnimMontage* UKRWeaponInstance::GetLightAttackMontage(int32 ComboIndex) const
{
    if (LightAttackMontages.IsValidIndex(ComboIndex))
    {
        return LightAttackMontages[ComboIndex];
    }
    return nullptr;
}

UAnimMontage* UKRWeaponInstance::GetChargeAttackMontage(int32 ComboIndex) const
{
    if (ChargeAttackMontages.IsValidIndex(ComboIndex))
    {
        return ChargeAttackMontages[ComboIndex];
    }
    return nullptr;
}

float UKRWeaponInstance::GetWeaponChargeTime() const
{
    return ChargeTime;
}

void UKRWeaponInstance::GrantWeaponAbilities(UKRAbilitySystemComponent* ASC)
{
    if (!ASC) return;

    RemoveWeaponAbilities(ASC);

    for (const FKRAbilitySet_GameplayAbility& AbilityInfo : CachedAbilities)
    {
        if (!AbilityInfo.Ability) continue;

        FGameplayAbilitySpec Spec(AbilityInfo.Ability, AbilityInfo.AbilityLevel, -1, this);

        if (AbilityInfo.InputTag.IsValid())
        {
            Spec.GetDynamicSpecSourceTags().AddTag(AbilityInfo.InputTag);
        }

        FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(Spec);
        GrantedHandles.AddAbilitySpecHandle(Handle);
    }
}

void UKRWeaponInstance::RemoveWeaponAbilities(UKRAbilitySystemComponent* ASC)
{
    if (!ASC) return;
    GrantedHandles.TakeFromAbilitySystem(ASC);
}

void UKRWeaponInstance::PlayEquipVFX()
{
    if (AKRWeaponBase* WeaponBase = Cast<AKRWeaponBase>(SpawnedWeaponActor))
    {
        WeaponBase->PlayEquipEffect();
    }
}

void UKRWeaponInstance::PlayUnequipVFX()
{
    if (AKRWeaponBase* WeaponBase = Cast<AKRWeaponBase>(SpawnedWeaponActor))
    {
        WeaponBase->PlayUnequipEffect();
    }
}

void UKRWeaponInstance::BroadcastWeaponMessage(const EWeaponMessageAction Action)
{
    if (UWorld* World = GetWorld())
    {
        UGameplayMessageSubsystem& Subsys = UGameplayMessageSubsystem::Get(World);

        FKRUIMessage_Weapon Payload;
        Payload.Action = Action;
        Payload.WeaponTypeTag = WeaponType;
        Payload.CurrentAmmo = CurrentAmmo;
        Payload.MaxAmmo = MaxAmmo;

        Subsys.BroadcastMessage(FKRUIMessageTags::Weapon(), Payload);
    }
}
*/
