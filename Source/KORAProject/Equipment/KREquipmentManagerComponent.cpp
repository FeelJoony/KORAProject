// #include "KREquipmentManagerComponent.h"
// #include "KREquipmentDefinition.h"
// #include "KREquipmentInstance.h"
// #include "AbilitySystemComponent.h"
// #include "Abilities/GameplayAbility.h"
//
// #include UE_INLINE_GENERATED_CPP_BY_NAME(KREquipmentManagerComponent)
//
// FKRAppliedEquipmentEntry* FKREquipmentList::AddEntry(TSubclassOf<UKREquipmentDefinition> InEquipmentDefinition)
// {
// 	check(InEquipmentDefinition != nullptr);
//  	check(OwnerComponent);
// 	
// 	const UKREquipmentDefinition* EquipmentCDO = GetDefault<UKREquipmentDefinition>(InEquipmentDefinition);
//
// 	TSubclassOf<UKREquipmentInstance> InstanceType = EquipmentCDO->InstanceType;
// 	if (InstanceType == nullptr)
// 	{
// 		InstanceType = UKREquipmentInstance::StaticClass();
// 	}
// 	
// 	FKRAppliedEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
// 	NewEntry.EquipmentDefinition = InEquipmentDefinition;
// 	NewEntry.Instance = NewObject<UKREquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);
// 	
// 	return &NewEntry;
// }
//
// void FKREquipmentList::RemoveEntry(UKREquipmentInstance* InInstance)
// {
// 	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
// 	{
// 		FKRAppliedEquipmentEntry& Entry = *EntryIt;
// 		if (Entry.Instance == InInstance)
// 		{
// 			EntryIt.RemoveCurrent();
// 			return;
// 		}
// 	}
// }
//
// FKRAppliedEquipmentEntry* FKREquipmentList::FindEntryByInstance(UKREquipmentInstance* InInstance)
// {
// 	if (InInstance)
// 	{
// 		for (FKRAppliedEquipmentEntry& Entry : Entries)
// 		{
// 			if (Entry.Instance == InInstance)
// 			{
// 				return &Entry;
// 			}
// 		}
// 	}
// 	return nullptr;
// }
//
//
// UKREquipmentManagerComponent::UKREquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
// 	: Super(ObjectInitializer)
// 	, EquipmentList(this)
// {
// 	bWantsInitializeComponent = true;
// }
//
// UAbilitySystemComponent* UKREquipmentManagerComponent::GetAbilitySystemComponent() const
// {
// 	AActor* Owner = GetOwner();
// 	if (Owner)
// 	{
// 		if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Owner))
// 		{
// 			return ASI->GetAbilitySystemComponent();
// 		}
// 	}
// 	return nullptr;
// }
//
// UKREquipmentInstance* UKREquipmentManagerComponent::EquipItem(TSubclassOf<UKREquipmentDefinition> InEquipmentDefinition)
// {
// 	if (InEquipmentDefinition == nullptr)
// 	{
// 		return nullptr;
// 	}
//
// 	const UKREquipmentDefinition* EquipmentCDO = GetDefault<UKREquipmentDefinition>(InEquipmentDefinition);
//
// 	const TArray<FGameplayTag>& SlotTagsToOccupy = EquipmentCDO->EquipmentSlotTags;
// 	if (!SlotTagsToOccupy.IsEmpty())
// 	{
// 		TArray<UKREquipmentInstance*> OldInstancesToUnequip;
// 		for (const FGameplayTag& SlotTag : SlotTagsToOccupy)
// 		{
// 			if (UKREquipmentInstance* OldInstance = EquippedSlotsMap.FindRef(SlotTag))
// 			{
// 				OldInstancesToUnequip.AddUnique(OldInstance);
// 			}
// 		}
//
// 		for (UKREquipmentInstance* OldInstance : OldInstancesToUnequip)
// 		{
// 			UnequipItem(OldInstance);
// 		}
// 	}
//
// 	FKRAppliedEquipmentEntry* NewEntry = EquipmentList.AddEntry(InEquipmentDefinition);
// 	UKREquipmentInstance* NewInstance = NewEntry->Instance;
// 	
// 	if (NewInstance != nullptr)
// 	{
// 		NewInstance->OnEquipped(EquipmentCDO->ActorsToSpawn);
// 		
// 		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
// 		{
// 			const TArray<TSubclassOf<UGameplayAbility>>& AbilitiesToGrant = EquipmentCDO->AbilitiesToGrant;
// 	
// 			for (const TSubclassOf<UGameplayAbility>& AbilityClass : AbilitiesToGrant)
// 			{
// 				if (AbilityClass)
// 				{
// 					FGameplayAbilitySpec AbilitySpec(AbilityClass, 1, -1, NewInstance);
// 					FGameplayAbilitySpecHandle AbilityHandle = ASC->GiveAbility(AbilitySpec);
// 					NewEntry->GrantedAbilityHandles.Add(AbilityHandle);
// 				}
// 			}
// 		}
//
// 		for (const FGameplayTag& SlotTag : SlotTagsToOccupy)
// 		{
// 			EquippedSlotsMap.Add(SlotTag, NewInstance);
// 		}
// 	}
// 	
// 	return NewInstance;
// }
//
// void UKREquipmentManagerComponent::UnequipItem(UKREquipmentInstance* InItemInstance)
// {
// 	if (InItemInstance == nullptr)
// 	{
// 		return;
// 	}
// 	
// 	FKRAppliedEquipmentEntry* Entry = EquipmentList.FindEntryByInstance(InItemInstance);
// 	if (Entry == nullptr)
// 	{
// 		return;
// 	}
//
// 	const TSubclassOf<UKREquipmentDefinition> EquipmentDefClass = Entry->EquipmentDefinition;
// 	const UKREquipmentDefinition* EquipmentCDO = GetDefault<UKREquipmentDefinition>(EquipmentDefClass);
// 	
// 	InItemInstance->OnUnequipped();
// 	
// 	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
// 	{
// 		for (const FGameplayAbilitySpecHandle& AbilityHandle : Entry->GrantedAbilityHandles)
// 		{
// 			ASC->ClearAbility(AbilityHandle);
// 		}
// 		Entry->GrantedAbilityHandles.Empty();
// 	}
//
// 	if (EquipmentCDO)
// 	{
// 		const TArray<FGameplayTag>& SlotTags = EquipmentCDO->EquipmentSlotTags;
// 		for (const FGameplayTag& SlotTag : SlotTags)
// 		{
// 			if (EquippedSlotsMap.FindRef(SlotTag) == InItemInstance)
// 			{
// 				EquippedSlotsMap.Remove(SlotTag);
// 			}
// 		}
// 	}
//
// 	EquipmentList.RemoveEntry(InItemInstance);
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
// UKREquipmentInstance* UKREquipmentManagerComponent::GetFirstInstanceOfType(TSubclassOf<UKREquipmentInstance> InstanceType) const
// {
// 	for (const FKRAppliedEquipmentEntry& Entry : EquipmentList.Entries)
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