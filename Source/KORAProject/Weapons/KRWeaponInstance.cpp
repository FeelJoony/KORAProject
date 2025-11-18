// #include "Weapons/KRWeaponInstance.h"
//
// #include "GameFramework/Pawn.h"
// #include "Engine/World.h"
// #include "Math/UnrealMathUtility.h"
// #include "Misc/AssertionMacros.h"
// #include "GameFramework/InputDeviceSubsystem.h"
// #include "GameFramework/InputDeviceProperties.h"
// //#include "Character/LyraHealthComponent.h"
//
// #include UE_INLINE_GENERATED_CPP_BY_NAME(KRWeaponInstance)
//
// class UAnimInstance;
// struct FGameplayTagContainer;
//
// UKRWeaponInstance::UKRWeaponInstance(const FObjectInitializer& ObjectInitializer)
// 	: Super(ObjectInitializer)
// {
// 	if (APawn* Pawn = GetPawn())
// 	{
// 		if (Pawn->IsPlayerControlled())
// 		{
// 			/* 플레이어가 무기를 사용하는지 판단하는 로직인거 같은데, Weapon이 플레이어만 사용한다면 굳이 필요 없으려나 감히 생각해봤습니다.
// 			if (ULyraHealthComponent* HealthComponent = ULyraHealthComponent::FindHealthComponent(GetPawn()))
// 			{
// 				HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
// 			}
// 			*/
// 		}
// 	}
// }
//
// void UKRWeaponInstance::OnEquipped(const TArray<FKREquipmentActorToSpawn>& ActorsToSpawn)
// {
// 	Super::OnEquipped(ActorsToSpawn);
//
// 	UWorld* World = GetWorld();
// 	check(World);
// 	TimeLastEquipped = World->GetTimeSeconds();
//
// 	ApplyDeviceProperties();
// }
//
// void UKRWeaponInstance::OnUnequipped()
// {
// 	Super::OnUnequipped();
//
// 	RemoveDeviceProperties();
// }
//
// void UKRWeaponInstance::UpdateFiringTime()
// {
// 	UWorld* World = GetWorld();
// 	check(World);
// 	TimeLastFired = World->GetTimeSeconds();
// }
//
// float UKRWeaponInstance::GetTimeSinceLastInteractedWith() const
// {
// 	UWorld* World = GetWorld();
// 	check(World);
// 	const double WorldTime = World->GetTimeSeconds();
//
// 	double Result = WorldTime - TimeLastEquipped;
//
// 	if (TimeLastFired > 0.0)
// 	{
// 		const double TimeSinceFired = WorldTime - TimeLastFired;
// 		Result = FMath::Min(Result, TimeSinceFired);
// 	}
//
// 	return Result;
// }
//
// TSubclassOf<UAnimInstance> UKRWeaponInstance::PickBestAnimLayer(bool bEquipped, const FGameplayTagContainer& CosmeticTags) const
// {
// 	/*
// 	const FKRAnimLayerSelectionSet& SetToQuery = (bEquipped ? EquippedAnimSet : UneuippedAnimSet);
// 	return SetToQuery.SelectBestLayer(CosmeticTags);
// 	*/
//
// 	return nullptr;
// }
//
// const FPlatformUserId UKRWeaponInstance::GetOwningUserId() const
// {
// 	if (const APawn* Pawn = GetPawn())
// 	{
// 		return Pawn->GetPlatformUserId();
// 	}
// 	return PLATFORMUSERID_NONE;
// }
//
// void UKRWeaponInstance::ApplyDeviceProperties()
// {
// 	const FPlatformUserId UserId = GetOwningUserId();
//
// 	if (UserId.IsValid())
// 	{
// 		if (UInputDeviceSubsystem* InputDeviceSubsystem = UInputDeviceSubsystem::Get())
// 		{
// 			for (TObjectPtr<UInputDeviceProperty>& DeviceProp : ApplicableDeviceProperties)
// 			{
// 				FActivateDevicePropertyParams Params = {};
// 				Params.UserId = UserId;
// 				
// 				Params.bLooping = true;
// 			
// 				DevicePropertyHandles.Emplace(InputDeviceSubsystem->ActivateDeviceProperty(DeviceProp, Params));
// 			}
// 		}	
// 	}
// }
//
// void UKRWeaponInstance::RemoveDeviceProperties()
// {
// 	const FPlatformUserId UserId = GetOwningUserId();
// 	
// 	if (UserId.IsValid() && !DevicePropertyHandles.IsEmpty())
// 	{
// 		if (UInputDeviceSubsystem* InputDeviceSubsystem = UInputDeviceSubsystem::Get())
// 		{
// 			InputDeviceSubsystem->RemoveDevicePropertyHandles(DevicePropertyHandles);
// 			DevicePropertyHandles.Empty();
// 		}
// 	}
// }
//
// void UKRWeaponInstance::OnDeathStarted(AActor* OwningActor)
// {
// 	RemoveDeviceProperties();
// }
