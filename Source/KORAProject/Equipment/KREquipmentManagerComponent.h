// #pragma once
//
// #include "CoreMinimal.h"
// #include "Components/PawnComponent.h"
// #include "GameFramework/Pawn.h"
// #include "Net/Serialization/FastArraySerializer.h"
//
// #include "KREquipmentManagerComponent.generated.h"
//
// class UActorComponent;
// class UKRAbilitySystemComponent;
// class UKREquipmentDefinition;
// class UKREquipmentInstance;
// class UKREquipmentManagerComponent;
// class UObject;
// struct FFrame;
// struct FKREquipmentList;
// struct FNetDeltaSerializeInfo;
// struct FReplicationFlags;
//
// USTRUCT(BlueprintType)
// struct FKRAppliedEquipmentEntry : public FFastArraySerializerItem
// {
// 	GENERATED_BODY()
//
// 	FKRAppliedEquipmentEntry()
// 	{}
//
// 	FString GetDebugString() const;
//
// private:
// 	friend FKREquipmentList;
// 	friend UKREquipmentManagerComponent;
//
// 	UPROPERTY()
// 	TSubclassOf<UKREquipmentDefinition> EquipmentDefinition;
//
// 	UPROPERTY()
// 	TObjectPtr<UKREquipmentInstance> Instance = nullptr;
//
// 	/*
// 	UPROPERTY(NotReplicated)
// 	FKRAbilitySet_GrantedHandles GrantedHandles;
// 	*/
// };
//
// USTRUCT(BlueprintType)
// struct FKREquipmentList : public FFastArraySerializer
// {
// 	GENERATED_BODY()
//
// 	FKREquipmentList()
// 		: OwnerComponent(nullptr)
// 	{
// 	}
//
// 	FKREquipmentList(UActorComponent* InOwnerComponent)
// 		: OwnerComponent(InOwnerComponent)
// 	{
// 	}
//
// public:
// 	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
// 	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
// 	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
//
// 	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
// 	{
// 		return FFastArraySerializer::FastArrayDeltaSerialize<FKRAppliedEquipmentEntry, FKREquipmentList>(Entries, DeltaParms, *this);
// 	}
//
// 	UKREquipmentInstance* AddEntry(TSubclassOf<UKREquipmentDefinition> EquipmentDefinition);
// 	void RemoveEntry(UKREquipmentInstance* Instance);
//
// private:
// 	UKRAbilitySystemComponent* GetAbilitySystemComponent() const;
//
// 	friend UKREquipmentManagerComponent;
// 	
// 	UPROPERTY()
// 	TArray<FKRAppliedEquipmentEntry> Entries;
//
// 	UPROPERTY(NotReplicated)
// 	TObjectPtr<UActorComponent> OwnerComponent;
// };
//
// template<>
// struct TStructOpsTypeTraits<FKREquipmentList> : public TStructOpsTypeTraitsBase2<FKREquipmentList>
// {
// 	enum { WithNetDeltaSerializer = true };
// };
//
// UCLASS(BlueprintType, Const)
// class KORAPROJECT_API UKREquipmentManagerComponent : public UPawnComponent
// {
// 	GENERATED_BODY()
// public:
// 	UKREquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
//
// 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
// 	UKREquipmentInstance* EquipItem(TSubclassOf<UKREquipmentDefinition> EquipmentDefinition);
//
// 	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
// 	void UnequipItem(UKREquipmentInstance* ItemInstance);
// 	
// 	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
// 	
// 	UFUNCTION(BlueprintCallable, BlueprintPure)
// 	UKREquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UKREquipmentInstance> InstanceType);
// 	
// 	UFUNCTION(BlueprintCallable, BlueprintPure)
// 	TArray<UKREquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<UKREquipmentInstance> InstanceType) const;
//
// 	template <typename T>
// 	T* GetFirstInstanceOfType()
// 	{
// 		return (T*)GetFirstInstanceOfType(T::StaticClass());
// 	}
//
// private:
// 	UPROPERTY(Replicated)
// 	FKREquipmentList EquipmentList;
// };
