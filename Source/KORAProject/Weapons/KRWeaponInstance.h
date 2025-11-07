#pragma once

#include "CoreMinimal.h"
#include "Equipment/KREquipmentInstance.h"
#include "GameFramework/InputDevicePropertyHandle.h"

#include "KRWeaponInstance.generated.h"

class UAnimInstance;
class UObject;
struct FFrame;
struct FGameplayTagContainer;
class UInputDeviceProperty;

UCLASS()
class KORAPROJECT_API UKRWeaponInstance : public UKREquipmentInstance
{
	GENERATED_BODY()

public:
	UKRWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void OnEquipped() override;
	virtual void OnUnequipped() override;

	UFUNCTION(BlueprintCallable)
	void UpdateFiringTime();

	UFUNCTION(BlueprintCallable)
	float GetTimeSinceLastInteractedWith() const;

protected:
	// 일단 효과로 처리한다고는 말씀하셨는데, 혹시 몰라서 남겨놨습니다.
	
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	//FKRAnimLayerSelectionSet EquippedAnimSet;
	
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Animation)
	//FKRAnimLayerSelectionSet UneuippedAnimSet;

	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "Input Devices")
	TArray<TObjectPtr<UInputDeviceProperty>> ApplicableDeviceProperties;

	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category=Animation)
	TSubclassOf<UAnimInstance> PickBestAnimLayer(bool bEquipped, const FGameplayTagContainer& CosmeticTags) const;

	UFUNCTION(BlueprintCallable)
	const FPlatformUserId GetOwningUserId() const;

	UFUNCTION()
	void OnDeathStarted(AActor* OwningActor);

	void ApplyDeviceProperties();

	void RemoveDeviceProperties();

private:
	UPROPERTY(Transient)
	TSet<FInputDevicePropertyHandle> DevicePropertyHandles;

	double TimeLastEquipped = 0.0;
	double TimeLastFired = 0.0;
};
