// // Copyright Epic Games, Inc. All Rights Reserved.
//
// #include "KREquipmentManagerComponent.h"
//
// //#include "AbilitySystem/KRAbilitySystemComponent.h"
// #include "AbilitySystemGlobals.h"
// #include "Engine/ActorChannel.h"
// #include "KREquipmentDefinition.h"
// #include "KREquipmentInstance.h"
// #include "Net/UnrealNetwork.h"
//
// #include UE_INLINE_GENERATED_CPP_BY_NAME(KREquipmentManagerComponent)
//
// class FLifetimeProperty;
// struct FReplicationFlags;
//
// FString FKRAppliedEquipmentEntry::GetDebugString() const
// {
// 	return FString::Printf(TEXT("%s of %s"), *GetNameSafe(Instance), *GetNameSafe(EquipmentDefinition.Get()));
// }
//
//
// void FKREquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
// {
//  	for (int32 Index : RemovedIndices)
//  	{
//  		const FKRAppliedEquipmentEntry& Entry = Entries[Index];
// 		if (Entry.Instance != nullptr)
// 		{
// 			Entry.Instance->OnUnequipped();
// 		}
//  	}
// }
//
// void FKREquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
// {
// 	for (int32 Index : AddedIndices)
// 	{
// 		const FKRAppliedEquipmentEntry& Entry = Entries[Index];
// 		if (Entry.Instance != nullptr)
// 		{
// 			Entry.Instance->OnEquipped();
// 		}
// 	}
// }
//
// void FKREquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
// {
// // 	for (int32 Index : ChangedIndices)
// // 	{
// // 		const FGameplayTagStack& Stack = Stacks[Index];
// // 		TagToCountMap[Stack.Tag] = Stack.StackCount;
// // 	}
// }
//
// /*
// UKRAbilitySystemComponent* FKREquipmentList::GetAbilitySystemComponent() const
// {
// 	check(OwnerComponent);
// 	AActor* OwningActor = OwnerComponent->GetOwner();
// 	return Cast<UKRAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor));
// }
// */
//
// UKREquipmentInstance* FKREquipmentList::AddEntry(TSubclassOf<UKREquipmentDefinition> EquipmentDefinition)
// {
// 	UKREquipmentInstance* Result = nullptr;
//
// 	check(EquipmentDefinition != nullptr);
//  	check(OwnerComponent);
// 	check(OwnerComponent->GetOwner()->HasAuthority());
// 	
// 	const UKREquipmentDefinition* EquipmentCDO = GetDefault<UKREquipmentDefinition>(EquipmentDefinition);
//
// 	TSubclassOf<UKREquipmentInstance> InstanceType = EquipmentCDO->InstanceType;
// 	if (InstanceType == nullptr)
// 	{
// 		InstanceType = UKREquipmentInstance::StaticClass();
// 	}
// 	
// 	FKRAppliedEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
// 	NewEntry.EquipmentDefinition = EquipmentDefinition;
// 	NewEntry.Instance = NewObject<UKREquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);  //@TODO: Using the actor instead of component as the outer due to UE-127172
// 	Result = NewEntry.Instance;
//
// 	if (UKRAbilitySystemComponent* ASC = GetAbilitySystemComponent())
// 	{
// 		for (const TObjectPtr<const UKRAbilitySet>& AbilitySet : EquipmentCDO->AbilitySetsToGrant)
// 		{
// 			AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &NewEntry.GrantedHandles, Result);
// 		}
// 	}
// 	else
// 	{
// 		//@TODO: Warning logging?
// 	}
//
// 	Result->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);
//
//
// 	MarkItemDirty(NewEntry);
//
// 	return Result;
// }
//
// void FKREquipmentList::RemoveEntry(UKREquipmentInstance* Instance)
// {
// 	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
// 	{
// 		FKRAppliedEquipmentEntry& Entry = *EntryIt;
// 		if (Entry.Instance == Instance)
// 		{
// 			if (UKRAbilitySystemComponent* ASC = GetAbilitySystemComponent())
// 			{
// 				Entry.GrantedHandles.TakeFromAbilitySystem(ASC);
// 			}
//
// 			Instance->DestroyEquipmentActors();
// 			
//
// 			EntryIt.RemoveCurrent();
// 			MarkArrayDirty();
// 		}
// 	}
// }
//
// UKREquipmentManagerComponent::UKREquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
// 	: Super(ObjectInitializer)
// 	, EquipmentList(this)
// {
// 	SetIsReplicatedByDefault(true);
// 	bWantsInitializeComponent = true;
// }
//
// void UKREquipmentManagerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
// {
// 	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
// 	DOREPLIFETIME(ThisClass, EquipmentList);
// }
//
// UKREquipmentInstance* UKREquipmentManagerComponent::EquipItem(TSubclassOf<UKREquipmentDefinition> EquipmentClass)
// {
// 	UKREquipmentInstance* Result = nullptr;
// 	if (EquipmentClass != nullptr)
// 	{
// 		Result = EquipmentList.AddEntry(EquipmentClass);
// 		if (Result != nullptr)
// 		{
// 			Result->OnEquipped();
//
// 			if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
// 			{
// 				AddReplicatedSubObject(Result);
// 			}
// 		}
// 	}
// 	return Result;
// }
//
// void UKREquipmentManagerComponent::UnequipItem(UKREquipmentInstance* ItemInstance)
// {
// 	if (ItemInstance != nullptr)
// 	{
// 		if (IsUsingRegisteredSubObjectList())
// 		{
// 			RemoveReplicatedSubObject(ItemInstance);
// 		}
//
// 		ItemInstance->OnUnequipped();
// 		EquipmentList.RemoveEntry(ItemInstance);
// 	}
// }
//
// bool UKREquipmentManagerComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
// {
// 	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
//
// 	for (FKRAppliedEquipmentEntry& Entry : EquipmentList.Entries)
// 	{
// 		UKREquipmentInstance* Instance = Entry.Instance;
//
// 		if (IsValid(Instance))
// 		{
// 			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
// 		}
// 	}
//
// 	return WroteSomething;
// }
//
// void UKREquipmentManagerComponent::InitializeComponent()
// {
// 	Super::InitializeComponent();
// }
//
// void UKREquipmentManagerComponent::UninitializeComponent()
// {
// 	TArray<UKREquipmentInstance*> AllEquipmentInstances;
//
// 	// gathering all instances before removal to avoid side effects affecting the equipment list iterator	
// 	for (const FKRAppliedEquipmentEntry& Entry : EquipmentList.Entries)
// 	{
// 		AllEquipmentInstances.Add(Entry.Instance);
// 	}
//
// 	for (UKREquipmentInstance* EquipInstance : AllEquipmentInstances)
// 	{
// 		UnequipItem(EquipInstance);
// 	}
//
// 	Super::UninitializeComponent();
// }
//
// void UKREquipmentManagerComponent::ReadyForReplication()
// {
// 	Super::ReadyForReplication();
// 	
// 	if (IsUsingRegisteredSubObjectList())
// 	{
// 		for (const FKRAppliedEquipmentEntry& Entry : EquipmentList.Entries)
// 		{
// 			UKREquipmentInstance* Instance = Entry.Instance;
//
// 			if (IsValid(Instance))
// 			{
// 				AddReplicatedSubObject(Instance);
// 			}
// 		}
// 	}
// }
//
// UKREquipmentInstance* UKREquipmentManagerComponent::GetFirstInstanceOfType(TSubclassOf<UKREquipmentInstance> InstanceType)
// {
// 	for (FKRAppliedEquipmentEntry& Entry : EquipmentList.Entries)
// 	{
// 		if (UKREquipmentInstance* Instance = Entry.Instance)
// 		{
// 			if (Instance->IsA(InstanceType))
// 			{
// 				return Instance;
// 			}
// 		}
// 	}
//
// 	return nullptr;
// }
//
// TArray<UKREquipmentInstance*> UKREquipmentManagerComponent::GetEquipmentInstancesOfType(TSubclassOf<UKREquipmentInstance> InstanceType) const
// {
// 	TArray<UKREquipmentInstance*> Results;
// 	for (const FKRAppliedEquipmentEntry& Entry : EquipmentList.Entries)
// 	{
// 		if (UKREquipmentInstance* Instance = Entry.Instance)
// 		{
// 			if (Instance->IsA(InstanceType))
// 			{
// 				Results.Add(Instance);
// 			}
// 		}
// 	}
// 	return Results;
// }
//
//
