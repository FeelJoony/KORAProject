#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "TableRowConvertFunctionContainer.generated.h"

struct FParseMethodParams
{
	const TMap<FName, int32> Headers;
	const TArray<TArray<FString>> Values;

	FParseMethodParams() : Headers(TMap<FName, int32> {}), Values(TArray<TArray<FString>> {}) {}
	FParseMethodParams(const TMap<FName, int32>& Headers, const TArray<TArray<FString>>& Values) : Headers(Headers), Values(Values) { }
};

DECLARE_DELEGATE_OneParam(FParseMethod, const FParseMethodParams& /*Params*/);

UCLASS()
class KORACUSTOMEDITORS_API UTableRowConvertFunctionContainer : public UObject
{
	GENERATED_BODY()
	
public:
	UTableRowConvertFunctionContainer();

	UFUNCTION()
	void CreateSampleData(class UDataTable* OutDataTable, const FString& InCSVString);
	UFUNCTION()
	void CreateItemData(class UDataTable* OutDataTable, const FString& InCSVString);

private:
	void OutHeaderAndValues(const FString& InCSVString, TMap<FName, int32>& OutHeaders, TArray<TArray<FString>>& OutValues, const FString& CSVFileName);
	void CreateData(const FString& InCSVString, const FString& CSVFileName, FParseMethod ParseMethodDelegate);

	int32 GetHeaderIndex(TMap<FName, int> Headers, const FName& HeaderName)
	{
		return Headers[HeaderName];
	}

	int32 ParseIntValue(const FString& ValueString) const
	{
		return FCString::Atoi(*ValueString);
	}

	float ParseFloatValue(const FString& ValueString) const
	{
		return FCString::Atof(*ValueString);
	}

	int64 ParseLongValue(const FString& ValueString) const
	{
		return FCString::Atoi64(*ValueString);
	}

	double ParseDoubleValue(const FString& ValueString) const
	{
		return FCString::Atod(*ValueString);
	}

	FText ParseTextValue(const FString& ValueString) const
	{
		return FText::FromString(ValueString);
	}

	FGameplayTag ParseGameplayTagValue(const FString& ValueString) const
	{
		return FGameplayTag::RequestGameplayTag(FName(*ValueString));
	}

	template<typename TEnum>
	TEnum ParseEnumValue(const FString& ValueString) const
	{
		const UEnum* EnumType = StaticEnum<TEnum>();
		check(EnumType);
		const int64 EnumValueInt = EnumType->GetValueByNameString(ValueString);

		return static_cast<TEnum>(EnumValueInt);
	}

	template<typename AssetType>
	TSoftObjectPtr<AssetType> ParseSoftObjectValue(const FString& ValueString)
	{
		FSoftObjectPath ResourcePath = FSoftObjectPath(ValueString);
		TSoftObjectPtr<AssetType> Resource(ResourcePath);
		
		return Resource;
	}

	TArray<FString> ParseArrayValue(const FString& ValueString)
	{
		TArray<FString> ValueArray;
		ValueString.ParseIntoArray(ValueArray, TEXT("|"));
		
		return ValueArray;
	}
};

