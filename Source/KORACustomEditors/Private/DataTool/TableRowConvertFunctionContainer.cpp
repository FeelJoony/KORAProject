#include "DataTool/TableRowConvertFunctionContainer.h"
#include "Data/SampleDataStruct.h"

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
