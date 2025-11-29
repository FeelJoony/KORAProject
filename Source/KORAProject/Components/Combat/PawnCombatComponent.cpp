#include "Components/Combat/PawnCombatComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "GameplayTag/KREventTag.h"          // KREventTag::Event_MeleeHit 등
#include "Item/Weapons/KRWeaponBase.h"
#include "Item/Weapons/KRMeleeWeapon.h"
#include "Item/Weapons/KRRangeWeapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapons/KRRangeWeaponInstance.h"
#include "Weapons/KRWeaponInstance.h"

class UKRRangeWeaponInstance;

UPawnCombatComponent::UPawnCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UPawnCombatComponent::BeginPlay()
{
    Super::BeginPlay();
    
    ASC = Cast<UKRAbilitySystemComponent>(
        UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()));
}

void UPawnCombatComponent::RegisterSpawnedWeapon(
    FGameplayTag InWeaponTagToRegister,
    AKRWeaponBase* InWeaponToRegister,
    EWeaponSlot InSlot,
    bool bRegisterAsEquippedWeapon)
{
    check(InWeaponToRegister);
    checkf(!CharacterCarriedWeaponMap.Contains(InWeaponTagToRegister),
        TEXT("A %s has already added carried weapon"), *InWeaponTagToRegister.ToString());

    CharacterCarriedWeaponMap.Emplace(InWeaponTagToRegister, InWeaponToRegister);
    SlotToWeaponMap.Emplace(InSlot, InWeaponToRegister);

    if (bRegisterAsEquippedWeapon)
    {
        CurrentEquippedWeaponTag = InWeaponTagToRegister;
        CurrentWeaponActor       = InWeaponToRegister;
        // Instance는 SetCurrentWeapon에서 따로 세팅
    }
}

void UPawnCombatComponent::SetCurrentWeapon(UKRWeaponInstance* InInstance, AKRWeaponBase* InActor)
{
    CurrentWeaponInstance = InInstance;
    CurrentWeaponActor    = InActor;
}

AKRWeaponBase* UPawnCombatComponent::GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const
{
    if (const TObjectPtr<AKRWeaponBase>* FoundWeapon = CharacterCarriedWeaponMap.Find(InWeaponTagToGet))
    {
        return FoundWeapon->Get();
    }
    return nullptr;
}

AKRWeaponBase* UPawnCombatComponent::GetCharacterCurrentEquippedWeapon() const
{
    if (!CurrentEquippedWeaponTag.IsValid())
    {
        return nullptr;
    }

    return GetCharacterCarriedWeaponByTag(CurrentEquippedWeaponTag);
}

AKRWeaponBase* UPawnCombatComponent::GetWeaponBySlot(EWeaponSlot InSlot) const
{
    if (const TObjectPtr<AKRWeaponBase>* Found = SlotToWeaponMap.Find(InSlot))
    {
        return Found->Get();
    }
    return nullptr;
}

void UPawnCombatComponent::ToggleWeaponCollision(bool bShouldEnable,
                                                 EToggleDamageType ToggleDamageType,
                                                 EWeaponSlot WeaponSlot)
{
    switch (ToggleDamageType)
    {
    case EToggleDamageType::CurrentEquippedWeapon:
        {
            TArray<AKRWeaponBase*> TargetWeapons;

            if (WeaponSlot == EWeaponSlot::AllHands)
            {
                for (const auto& Pair : SlotToWeaponMap)
                {
                    if (Pair.Value)
                    {
                        TargetWeapons.Add(Pair.Value.Get());
                    }
                }
            }
            else
            {
                if (AKRWeaponBase* Weapon = GetWeaponBySlot(WeaponSlot))
                {
                    TargetWeapons.Add(Weapon);
                }
            }

            for (AKRWeaponBase* WeaponBase : TargetWeapons)
            {
                if (!WeaponBase) continue;

                if (AKRMeleeWeapon* MeleeWeapon = Cast<AKRMeleeWeapon>(WeaponBase))
                {
                    MeleeWeapon->SetHitCollisionEnabled(bShouldEnable);
                }
            }

            if (!bShouldEnable)
            {
                OverlappedActors.Empty();
            }
        }
        break;

    case EToggleDamageType::Body:
        {
            // TODO: 캐릭터 몸통, 팔, 다리 콜리전 토글 (캡슐/스켈레탈 콜리전 세트)
        }
        break;
    }
}


void UPawnCombatComponent::HandleMeleeHit(AActor* HitActor, const FHitResult& Hit)
{
    if (!CurrentWeaponInstance || !HitActor || !GetOwner()->HasAuthority())
        return;

    if (OverlappedActors.Contains(HitActor))
    {
        return;
    }
    OverlappedActors.Add(HitActor);

    bool bIsCritical = false;
    const float Damage = CurrentWeaponInstance->CalculateDamage(bIsCritical);

    ApplyDamageToTarget(HitActor, Damage, bIsCritical, &Hit);
}

bool UPawnCombatComponent::FireRangedWeapon()
{
    if (!CurrentWeaponInstance || !CurrentWeaponActor || !GetOwner()->HasAuthority())
        return false;

    UKRRangeWeaponInstance* RangeInstance = Cast<UKRRangeWeaponInstance>(CurrentWeaponInstance);
    if (!RangeInstance)
        return false;

    // 탄약 체크
    if (!RangeInstance->ConsumeAmmo(1))
        return false;

    if (AKRRangeWeapon* RangeWeaponActor = Cast<AKRRangeWeapon>(CurrentWeaponActor))
    {
        RangeWeaponActor->FireProjectile();
    }

    const FTransform MuzzleTM = CurrentWeaponActor->GetMuzzleTransform();
    const FVector Start = MuzzleTM.GetLocation();

    const float Range  = CurrentWeaponInstance->Range;
    const float Spread = RangeInstance->GetSpreadAngleDegrees();

    const FRotator BaseRot = MuzzleTM.GetRotation().Rotator();
    const FRotator RandomRot = UKismetMathLibrary::RandomRotator(true);
    const FRotator FinalRot = BaseRot + FRotator(
        (RandomRot.Pitch - 180.f) * Spread / 180.f,
        (RandomRot.Yaw   - 180.f) * Spread / 180.f,
        0.f
    );

    const FVector End = Start + FinalRot.Vector() * Range;

    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(WeaponFire), false, GetOwner());
    const bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit, Start, End, ECC_Visibility, Params);

    if (bHit && Hit.GetActor())
    {
        bool bIsCritical = false;
        const float Damage = CurrentWeaponInstance->CalculateDamage(bIsCritical);
        ApplyDamageToTarget(Hit.GetActor(), Damage, bIsCritical, &Hit);
    }

    // TODO: 탄피, 이펙트, 사운드 등

    return true;
}

// KRMeleeWeapon에서 여기로 연결하고 싶으면 OnHitTargetActor를 래핑으로 써도 됨
void UPawnCombatComponent::OnHitTargetActor(AActor* HitActor, const FHitResult& HitResult)
{
    HandleMeleeHit(HitActor, HitResult);
}

void UPawnCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor, const FHitResult& HitResult)
{
    // 필요하면 "무기 빼낼 때" 로직 구현 (예: 피격 상태 해제, 이펙트 등)
}

void UPawnCombatComponent::ApplyDamageToTarget(AActor* TargetActor, float BaseDamage, bool bIsCritical, const FHitResult* HitResult)
{
    if (!ASC || !TargetActor)
        return;

    FGameplayEventData EventData;
    EventData.Instigator = GetOwner();
    EventData.Target = TargetActor;
    EventData.EventMagnitude = BaseDamage;
    EventData.ContextHandle = ASC->MakeEffectContext();

    if (HitResult)
    {
        EventData.ContextHandle.AddHitResult(*HitResult);
    }

    // 필요하다면 PayloadTag, Object 등 추가 세팅 가능
    
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
        GetOwner(),
        KRTAG_EVENT_COMBAT_HIT,  // 프로젝트에서 정의한 Hit용 GameplayTag
        EventData
    );
}