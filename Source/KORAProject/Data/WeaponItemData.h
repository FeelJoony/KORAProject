#pragma once

#include "CoreMinimal.h"
#include "WeaponItemData.generated.h"

USTRUCT(BlueprintType)
struct KORAPROJECT_API FWeaponItemData
{
	GENERATED_BODY()

public:
	FWeaponItemData()
		: Index(0)
		, Width(0.f)
		, Height(0.f)
	{

	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Index;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Width;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Height;
};
