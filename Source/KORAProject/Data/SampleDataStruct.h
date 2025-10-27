#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TestSampleData.h"
#include "Interface/TableKey.h"
#include "SampleDataStruct.generated.h"

UENUM(BlueprintType)
enum class ESampleType : uint8
{
	Sample1,
	Sample2,
	Sample3,
	Sample4,
	Sample5
};

USTRUCT(BlueprintType)
struct KORAPROJECT_API FSampleDataStruct : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()

public:
	FSampleDataStruct()
		: Index(1)
		, SampleName(TEXT("SampleName"))
		, Width(0.f)
		, Height(0.f)
	{

	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sample Data")
	int32 Index;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sample Data")
	FString SampleName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sample Data")
	float Width;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sample Data")
	float Height;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sample Data")
	ESampleType SampleType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sample Data")
	FTestSampleData TestSampleData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sample Data")
	TArray<FTestSampleData> TestSampleDataArray;

	virtual int32 GetKey() const override
	{
		return Index;
	}
};
