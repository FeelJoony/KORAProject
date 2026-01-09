#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "WorldAreaVolume.generated.h"

class UBoxComponent;
class UKRWorldEventComponent;

UCLASS()
class KORAPROJECT_API AWorldAreaVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	AWorldAreaVolume();

protected:
	virtual void BeginPlay() override;

	/** 충돌 볼륨 */
	UPROPERTY(VisibleAnywhere, Category = "Area")
	TObjectPtr<UBoxComponent> BoxComponent;

	/** 진입 시 발생할 월드 이벤트 */
	UPROPERTY(EditAnywhere, Category = "Area")
	FGameplayTag AreaVolumeTag;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
};
