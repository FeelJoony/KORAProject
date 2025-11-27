#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "KRPawnExtensionComponent.generated.h"

UCLASS()
class KORAPROJECT_API UKRPawnExtensionComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UKRPawnExtensionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, Category = "KR|Pawn")
	TObjectPtr<const class UKRPawnData> PawnData;
	
	static const FName NAME_ActorFeatureName;

	static UKRPawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UKRPawnExtensionComponent>() : nullptr); }

	template<typename T>
	FORCEINLINE const T* GetPawnData() const { return Cast<T>(PawnData); }
	void SetPawnData(const class UKRPawnData* NewPawnData);
	
};
