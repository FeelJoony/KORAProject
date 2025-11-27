#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "KRPawnExtensionComponent.generated.h"

class UKRAbilitySystemComponent;
class UKRPawnData;
class UGameFrameworkComponentManager;
struct FActorInitStateChangedParams;

UCLASS()
class KORAPROJECT_API UKRPawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UKRPawnExtensionComponent(const FObjectInitializer& ObjectInitializer);

	static const FName NAME_ActorFeatureName;

	//~ Begin IGameFrameworkInitStateInterface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface

	UFUNCTION(BlueprintCallable, Category = "KR|Pawn")
	UKRAbilitySystemComponent* GetKRAbilitySystemComponent() const { return AbilitySystemComponent; }
	
	UFUNCTION(BlueprintPure, Category = "KR|Pawn")
	static UKRPawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UKRPawnExtensionComponent>() : nullptr); }

	template<class T>
	const T* GetPawnData() const {return Cast<T>(PawnData); }

	void SetPawnData(const UKRPawnData* InPawnData);
	
	void InitializeAbilitySystem(UKRAbilitySystemComponent* InASC, AActor* InOwnerActor);
	void UninitializeAbilitySystem();
	void HandleControllerChanged();
	void HandlePlayerStateReplicated();
	void SetupPlayerInputComponent();

	FSimpleMulticastDelegate OnAbilitySystemInitialized;
	FSimpleMulticastDelegate OnAbilitySystemUninitialized;
	
protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY(EditInstanceOnly, ReplicatedUsing = OnRep_PawnData, Category = "KR|Pawn")
	TObjectPtr<const UKRPawnData> PawnData;

	UPROPERTY(Transient)
	TObjectPtr<UKRAbilitySystemComponent> AbilitySystemComponent;
	
	UFUNCTION()
	void OnRep_PawnData();
};
