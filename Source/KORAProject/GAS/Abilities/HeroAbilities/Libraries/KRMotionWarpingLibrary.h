#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KRMotionWarpingLibrary.generated.h"

class UMotionWarpingComponent;

/**
 * Motion Warping Blueprint Library
 * GA나 Blueprint에서 쉽게 Motion Warping Target을 설정할 수 있는 Utility
 */
UCLASS()
class KORAPROJECT_API UKRMotionWarpingLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Motion Warp Target 설정 (위치 + 회전)
	 * @param Actor MotionWarpingComponent를 가진 액터
	 * @param TargetName 몽타주의 NotifyState와 매칭되는 이름
	 * @param Location 목표 위치
	 * @param Rotation 목표 회전
	 * @return 성공 여부
	 */
	UFUNCTION(BlueprintCallable, Category = "KR|MotionWarping")
	static bool SetWarpTarget(AActor* Actor, FName TargetName, FVector Location, FRotator Rotation);

	/**
	 * Motion Warp Target 설정 (위치만, 회전은 현재 유지)
	 */
	UFUNCTION(BlueprintCallable, Category = "KR|MotionWarping")
	static bool SetWarpTargetLocation(AActor* Actor, FName TargetName, FVector Location);

	/**
	 * Motion Warp Target 설정 (회전만, 위치는 현재 유지)
	 */
	UFUNCTION(BlueprintCallable, Category = "KR|MotionWarping")
	static bool SetWarpTargetRotation(AActor* Actor, FName TargetName, FRotator Rotation);
	
	/**
	 * 타겟 액터를 향해 Warp Target 설정
	 * @param Actor 이동할 액터
	 * @param TargetName Warp Target 이름
	 * @param TargetActor 목표 액터
	 * @param DistanceOffset 타겟과의 거리 오프셋 (양수: 멀리, 음수: 가까이)
	 * @param bFaceTarget 타겟을 바라보도록 회전할지
	 */
	UFUNCTION(BlueprintCallable, Category = "KR|MotionWarping")
	static bool SetWarpTargetToActor(AActor* Actor, FName TargetName, AActor* TargetActor,
		float DistanceOffset = 0.f, bool bFaceTarget = true);
	
	UFUNCTION(BlueprintCallable, Category = "KR|MotionWarping") // Warp Target to certain socket
	static bool SetWarpTargetToSocket(AActor* Actor, FName TargetName, AActor* TargetActor,
		FName SocketName, float DistanceOffset = 0.f, bool bFaceTarget = true);
	
	/**
	 * 방향과 거리로 Warp Target 설정 (넉백용)
	 * @param Actor 이동할 액터
	 * @param TargetName Warp Target 이름
	 * @param Direction 이동 방향 (정규화됨)
	 * @param Distance 이동 거리
	 * @param bFaceDirection 이동 방향을 바라볼지
	 */
	UFUNCTION(BlueprintCallable, Category = "KR|MotionWarping")
	static bool SetWarpTargetByDirection(AActor* Actor, FName TargetName, FVector Direction,
		float Distance, bool bFaceDirection = false);

	/**
	 * 공격자로부터 밀려나는 Warp Target 설정 (피격 넉백용)
	 * @param Victim 피격자
	 * @param TargetName Warp Target 이름
	 * @param Attacker 공격자
	 * @param KnockbackDistance 넉백 거리
	 * @param bFaceAttacker 공격자를 바라볼지
	 */
	UFUNCTION(BlueprintCallable, Category = "KR|MotionWarping")
	static bool SetKnockbackWarpTarget(AActor* Victim, FName TargetName, AActor* Attacker,
		float KnockbackDistance, bool bFaceAttacker = true);


	UFUNCTION(BlueprintCallable, Category = "KR|MotionWarping")
	static bool RemoveWarpTarget(AActor* Actor, FName TargetName);

	UFUNCTION(BlueprintCallable, Category = "KR|MotionWarping")
	static bool RemoveAllWarpTargets(AActor* Actor);

	UFUNCTION(BlueprintPure, Category = "KR|MotionWarping")
	static UMotionWarpingComponent* GetMotionWarpingComponent(AActor* Actor);

	UFUNCTION(BlueprintPure, Category = "KR|MotionWarping")
	static FVector GetDirectionToActor(AActor* FromActor, AActor* ToActor, bool bIgnoreZ = true);

	UFUNCTION(BlueprintPure, Category = "KR|MotionWarping")
	static FRotator GetRotationFromDirection(FVector Direction);
};
