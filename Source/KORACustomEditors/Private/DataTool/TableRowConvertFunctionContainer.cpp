#include "DataTool/TableRowConvertFunctionContainer.h"
#include "UObject/SoftObjectPath.h"
#include "UObject/SoftObjectPtr.h"
#include "Engine/Texture2D.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "Data/SampleDataStruct.h"
#include "Data/WeaponItemDataStruct.h"

UTableRowConvertFunctionContainer::UTableRowConvertFunctionContainer()
{

}

void UTableRowConvertFunctionContainer::CreateSampleData(UDataTable* OutDataTable, const FString& InCSVString)
{
    CreateData(InCSVString, FString(TEXT("SampleData")), FParseMethod::CreateLambda([&](FParseMethodParams Params)
        {
            auto& Headers = const_cast<TMap<FName, int32>&>(Params.Headers);
            auto& Values = const_cast<TArray<TArray<FString>>&>(Params.Values);

            for (int32 i = 0; i < Values.Num(); i++)
            {
                TArray<FString>& RowValue = Values[i];

                int32 Index_Index = GetHeaderIndex(Headers, TEXT("Index"));
                int32 SampleName_Index = GetHeaderIndex(Headers, TEXT("SampleName"));
                int32 Width_Index = GetHeaderIndex(Headers, TEXT("Width"));
                int32 Height_Index = GetHeaderIndex(Headers, TEXT("Height"));
                int32 SampleEnum_Index = GetHeaderIndex(Headers, TEXT("SampleEnum"));
                int32 MaxElementCount_Index = GetHeaderIndex(Headers, TEXT("MaxElementCount"));

                FSampleDataStruct SampleData;

                SampleData.Index = ParseIntValue(RowValue[Index_Index]);
                SampleData.SampleName = RowValue[SampleName_Index];
                SampleData.Width = ParseFloatValue(RowValue[Width_Index]);
                SampleData.Height = ParseFloatValue(RowValue[Height_Index]);
                SampleData.SampleType = ParseEnumValue<ESampleType>(RowValue[SampleEnum_Index]);
                SampleData.TestSampleData = FTestSampleData();
                SampleData.TestSampleData.Index = SampleData.Index;
                SampleData.TestSampleData.Width = SampleData.Width;
                SampleData.TestSampleData.Height = SampleData.Height;

                for (int32 j = 1; j <= MaxElementCount_Index; j++)
                {
                    FTestSampleData SampleArrayData = FTestSampleData();

                    int32 TestIndex_Index = GetHeaderIndex(Headers, *FString::Printf(TEXT("SampleIndex%d"), j));
                    int32 TestWidth_Index = GetHeaderIndex(Headers, *FString::Printf(TEXT("SampleWidth%d"), j));
                    int32 TestHeight_Index = GetHeaderIndex(Headers, *FString::Printf(TEXT("SampleHeight%d"), j));

                    SampleArrayData.Index = ParseIntValue(RowValue[TestIndex_Index]);
                    SampleArrayData.Width = ParseFloatValue(RowValue[TestWidth_Index]);
                    SampleArrayData.Height = ParseFloatValue(RowValue[TestHeight_Index]);

                    SampleData.TestSampleDataArray.Add(SampleArrayData);
                }

                OutDataTable->AddRow(*FString::Printf(TEXT("Sample_%d"), i), SampleData);
            }
        }));
}

void UTableRowConvertFunctionContainer::CreateWeaponItemData(UDataTable* OutDataTable, const FString& InCSVString)
{
    CreateData(InCSVString, FString(TEXT("WeaponItemData")), FParseMethod::CreateLambda([&](FParseMethodParams Params)
        {
            auto& Headers = const_cast<TMap<FName, int32>&>(Params.Headers);
            auto& Values = const_cast<TArray<TArray<FString>>&>(Params.Values);

            for (int32 i = 0; i < Values.Num(); i++)
            {
                TArray<FString>& RowValue = Values[i];

                FWeaponItemDataStruct OutRow;

                auto Get = [&](const TCHAR* Key) -> FString
                    {
                        int32 Idx = GetHeaderIndex(Headers, Key);
                        return RowValue.IsValidIndex(Idx) ? RowValue[Idx] : FString();
                    };

                OutRow.Index = ParseIntValue(Get(TEXT("Index")));
                OutRow.ItemID = FName(*Get(TEXT("ItemID")));

                OutRow.DisplayName = FText::FromString(Get(TEXT("DisplayName")));
                OutRow.Description = FText::FromString(Get(TEXT("Description")));

               /* OutRow.ItemIcon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(Get(TEXT("ItemIcon"))));

                OutRow.TypeTag = FGameplayTag::RequestGameplayTag(*Get(TEXT("TypeTag")));
                OutRow.RarityTag = FGameplayTag::RequestGameplayTag(*Get(TEXT("RarityTag")));*/

                /*{
                    FString TagsString = Get(TEXT("AbilityTags"));
                    TArray<FString> TagStrings;
                    TagsString.ParseIntoArray(TagStrings, TEXT("|"));

                    for (const FString& TagStr : TagStrings)
                    {
                        if (!TagStr.IsEmpty())
                        {
                            OutRow.AbilityTags.Add(FGameplayTag::RequestGameplayTag(*TagStr));
                        }
                    }
                }*/

                OutRow.BasePrice = ParseIntValue(Get(TEXT("BasePrice")));
                OutRow.StackMax = ParseIntValue(Get(TEXT("StackMax")));

                OutRow.BaseATK = ParseIntValue(Get(TEXT("BaseATK")));
                OutRow.BaseCritChance = ParseFloatValue(Get(TEXT("BaseCritChance")));
                OutRow.BaseCritMulti = ParseFloatValue(Get(TEXT("BaseCritMulti")));
                //OutRow.BaseRange = ParseFloatValue(Get(TEXT("BaseRange")));
                /*OutRow.BaseAccuracy = ParseFloatValue(Get(TEXT("BaseAccuracy")));
                OutRow.BaseRecoil = ParseFloatValue(Get(TEXT("BaseRecoil")));
                OutRow.BaseCapacity = ParseIntValue(Get(TEXT("BaseCapacity")));
                OutRow.BaseEnforceCost = ParseIntValue(Get(TEXT("BaseEnforceCost")));*/
                OutRow.ReloadTime = ParseFloatValue(Get(TEXT("ReloadTime")));
                //OutRow.AttackSpeed = ParseFloatValue(Get(TEXT("AttackSpeed")));

                OutDataTable->AddRow(*FString::Printf(TEXT("Weapon_%d"), OutRow.Index), OutRow);
            }
        }));
}


void UTableRowConvertFunctionContainer::OutHeaderAndValues(const FString& InCSVString, TMap<FName, int32>& OutHeaders, TArray<TArray<FString>>& OutValues, const FString& CSVFileName)
{
    TArray<FString> Lines;
    InCSVString.ParseIntoArrayLines(Lines);

    if (Lines.Num() < 1)
    {
        UE_LOG(LogTemp, Warning, TEXT("CSV file is empty: %s"), *CSVFileName);
        return;
    }

    FString HeaderLine = Lines[0];
    TArray<FString> HeaderArray;
    HeaderLine.ParseIntoArray(HeaderArray, TEXT(","), true);
    for (int32 i = 0; i < HeaderArray.Num(); i++)
    {
        HeaderArray[i] = HeaderArray[i].TrimStartAndEnd();
        OutHeaders.Add(*HeaderArray[i], i);
    }

    if (OutHeaders.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("CSV file has no headers: %s"), *CSVFileName);
        return;
    }

    for (int32 i = 1; i < Lines.Num(); ++i)
    {
        FString CurrentLine = Lines[i];
        if (CurrentLine.TrimStartAndEnd().IsEmpty())
        {
            continue;
        }

        TArray<FString> LineValues;
        CurrentLine.ParseIntoArray(LineValues, TEXT(","), true);

        OutValues.Add(LineValues);

        if (OutValues.Num() != OutHeaders.Num())
        {
            UE_LOG(LogTemp, Warning, TEXT("Line %d in %s has %d columns, expected %d. Skipping line."), i + 1, *CSVFileName, OutValues.Num(), OutHeaders.Num());
        }
    }
}

void UTableRowConvertFunctionContainer::CreateData(const FString& InCSVString, const FString& CSVFileName, FParseMethod ParseMethodDelegate)
{
	TMap<FName, int32> Headers;
	TArray<TArray<FString>> Values;
	OutHeaderAndValues(InCSVString, Headers, Values, CSVFileName);

	ParseMethodDelegate.ExecuteIfBound(FParseMethodParams(Headers, Values));
}
