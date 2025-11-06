#include "DataTool/TableRowConvertFunctionContainer.h"
#include "UObject/SoftObjectPath.h"
#include "UObject/SoftObjectPtr.h"
#include "Engine/Texture2D.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "Data/SampleDataStruct.h"
#include "Data/WeaponItemDataStruct.h"
#include "Data/ConsumeItemDataStruct.h"
#include "Data/MaterialItemDataStruct.h"

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

                FName RowName = *FString::Printf(TEXT("Sample_%d"), i);
                if (FSampleDataStruct* FindRow = OutDataTable->FindRow<FSampleDataStruct>(RowName, TEXT("")))
                {
                    *FindRow = SampleData;
                }
                else
                {
                    OutDataTable->AddRow(RowName, SampleData);
                }
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

                // 안전한 컬럼 접근 매크로
                auto Get = [&](const TCHAR* Key) -> FString
                    {
                        int32 Idx = GetHeaderIndex(Headers, Key);
                        return RowValue.IsValidIndex(Idx) ? RowValue[Idx] : FString();
                    };

                OutRow.Index = ParseIntValue(Get(TEXT("Index")));
                OutRow.ItemID = FName(*Get(TEXT("ItemID")));
                OutRow.DisplayName = FText::FromString(Get(TEXT("DisplayName")));
                OutRow.Description = FText::FromString(Get(TEXT("Description")));

                // Gameplay Tags
                auto ParseTag = [&](const TCHAR* Key) -> FGameplayTag
                    {
                        FString TagStr = Get(Key);
                        return TagStr.IsEmpty() ? FGameplayTag() :
                            FGameplayTag::RequestGameplayTag(FName(*TagStr));
                    };

                OutRow.TypeTag = ParseTag(TEXT("TypeTag"));
                OutRow.RarityTag = ParseTag(TEXT("RarityTag"));

                // AbilityTags parsing
                {
                    FString TagsString = Get(TEXT("AbilityTags"));
                    TArray<FString> TagStrings;
                    TagsString.ParseIntoArray(TagStrings, TEXT(";"));

                    for (const FString& TagStr : TagStrings)
                    {
                        if (!TagStr.IsEmpty())
                        {
                            OutRow.AbilityTags.Add(FGameplayTag::RequestGameplayTag(FName(*TagStr)));
                        }
                    }
                }

                // Icon Path
                FString IconString = Get(TEXT("ItemIcon"));
                if (!IconString.IsEmpty())
                {
                    OutRow.ItemIcon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(IconString));
                }

                // Numeric Stats
                OutRow.BasePrice = ParseIntValue(Get(TEXT("BasePrice")));
                OutRow.StackMax = ParseIntValue(Get(TEXT("StackMax")));

                OutRow.BaseATK = ParseIntValue(Get(TEXT("BaseATK")));
                OutRow.BaseCritChance = ParseFloatValue(Get(TEXT("BaseCritChance")));
                OutRow.BaseCritMulti = ParseFloatValue(Get(TEXT("BaseCritMulti")));

                OutRow.BaseRange = ParseFloatValue(Get(TEXT("BaseRange")));
                OutRow.BaseAccuracy = ParseFloatValue(Get(TEXT("BaseAccuracy")));
                OutRow.BaseRecoil = ParseFloatValue(Get(TEXT("BaseRecoil")));
                OutRow.BaseCapacity = ParseIntValue(Get(TEXT("BaseCapacity")));
                OutRow.BaseEnforceCost = ParseIntValue(Get(TEXT("BaseEnforceCost")));

                OutRow.ReloadTime = ParseFloatValue(Get(TEXT("ReloadTime")));
                OutRow.AttackSpeed = ParseFloatValue(Get(TEXT("AttackSpeed")));

                // Commit to DataTable
                FName RowName = *FString::Printf(TEXT("Weapon_%d"), OutRow.Index);

                if (FWeaponItemDataStruct* FindRow = OutDataTable->FindRow<FWeaponItemDataStruct>(RowName, TEXT("")))
                {
                    *FindRow = OutRow;
                }
                else
                {
                    OutDataTable->AddRow(RowName, OutRow);
                }
            }
        }));
}


void UTableRowConvertFunctionContainer::CreateConsumeItemData(UDataTable* OutDataTable, const FString& InCSVString)
{
    CreateData(InCSVString, FString(TEXT("ConsumeItemData")), FParseMethod::CreateLambda([&](FParseMethodParams Params)
        {
            auto& Headers = const_cast<TMap<FName, int32>&>(Params.Headers);
            auto& Values = const_cast<TArray<TArray<FString>>&>(Params.Values);

            for (int32 i = 0; i < Values.Num(); i++)
            {
                TArray<FString>& RowValue = Values[i];

                int32 Index_Index = GetHeaderIndex(Headers, TEXT("Index"));
                int32 ItemID_Index = GetHeaderIndex(Headers, TEXT("ItemID"));
                int32 TypeTag_Index = GetHeaderIndex(Headers, TEXT("TypeTag"));
                int32 RarityTag_Index = GetHeaderIndex(Headers, TEXT("RarityTag"));
                int32 AbilityTags_Index = GetHeaderIndex(Headers, TEXT("AbilityTags")); // ';'구분 리스트
                int32 DisplayName_Index = GetHeaderIndex(Headers, TEXT("DisplayName"));
                int32 Description_Index = GetHeaderIndex(Headers, TEXT("Description"));
                int32 Icon_Index = GetHeaderIndex(Headers, TEXT("ItemIcon"));
                int32 BasePrice_Index = GetHeaderIndex(Headers, TEXT("BasePrice"));
                int32 StackMax_Index = GetHeaderIndex(Headers, TEXT("StackMax"));
                int32 PoolTag_Index = GetHeaderIndex(Headers, TEXT("PoolTag"));

                FConsumeItemDataStruct ConsumeData;

                ConsumeData.Index = ParseIntValue(RowValue[Index_Index]);
                ConsumeData.ItemID = FName(*RowValue[ItemID_Index]);

                ConsumeData.TypeTag = FGameplayTag::RequestGameplayTag(FName(*RowValue[TypeTag_Index]));
                ConsumeData.RarityTag = FGameplayTag::RequestGameplayTag(FName(*RowValue[RarityTag_Index]));
                ConsumeData.PoolTag = FGameplayTag::RequestGameplayTag(FName(*RowValue[PoolTag_Index]));

                // AbilityTags CSV에서 분리 (예: "Tag1;Tag2;Tag3")
                TArray<FString> AbilityStrList;
                RowValue[AbilityTags_Index].ParseIntoArray(AbilityStrList, TEXT(";"), true);
                for (const FString& TagStr : AbilityStrList)
                {
                    ConsumeData.AbilityTags.Add(FGameplayTag::RequestGameplayTag(FName(*TagStr)));
                }

                ConsumeData.DisplayName = FText::FromString(RowValue[DisplayName_Index]);
                ConsumeData.Description = FText::FromString(RowValue[Description_Index]);

                FString IconString = RowValue[Icon_Index];
                ConsumeData.ItemIcon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(IconString));

                ConsumeData.BasePrice = ParseIntValue(RowValue[BasePrice_Index]);
                ConsumeData.StackMax = ParseIntValue(RowValue[StackMax_Index]);

                FName RowName = *FString::Printf(TEXT("Consume_%d"), i);
                if (FConsumeItemDataStruct* FindRow = OutDataTable->FindRow<FConsumeItemDataStruct>(RowName, TEXT("")))
                {
                    *FindRow = ConsumeData;
                }
                else
                {
                    OutDataTable->AddRow(RowName, ConsumeData);
                }
            }
        }));
}

void UTableRowConvertFunctionContainer::CreateMaterialItemData(UDataTable* OutDataTable, const FString& InCSVString)
{
    CreateData(InCSVString, FString(TEXT("MaterialItemData")), FParseMethod::CreateLambda([&](FParseMethodParams Params)
        {
            auto& Headers = const_cast<TMap<FName, int32>&>(Params.Headers);
            auto& Values = const_cast<TArray<TArray<FString>>&>(Params.Values);

            for (int32 i = 0; i < Values.Num(); i++)
            {
                TArray<FString>& RowValue = Values[i];

                int32 Index_Index = GetHeaderIndex(Headers, TEXT("Index"));
                int32 ItemID_Index = GetHeaderIndex(Headers, TEXT("ItemID"));
                int32 TypeTag_Index = GetHeaderIndex(Headers, TEXT("TypeTag"));
                int32 RarityTag_Index = GetHeaderIndex(Headers, TEXT("RarityTag"));
                int32 AbilityTags_Index = GetHeaderIndex(Headers, TEXT("AbilityTags")); // ';'구분 리스트
                int32 DisplayName_Index = GetHeaderIndex(Headers, TEXT("DisplayName"));
                int32 Description_Index = GetHeaderIndex(Headers, TEXT("Description"));
                int32 Icon_Index = GetHeaderIndex(Headers, TEXT("ItemIcon"));
                int32 BasePrice_Index = GetHeaderIndex(Headers, TEXT("BasePrice"));
                int32 StackMax_Index = GetHeaderIndex(Headers, TEXT("StackMax"));

                FMaterialItemDataStruct MaterialData;

                MaterialData.Index = ParseIntValue(RowValue[Index_Index]);
                MaterialData.ItemID = FName(*RowValue[ItemID_Index]);

                MaterialData.TypeTag = FGameplayTag::RequestGameplayTag(FName(*RowValue[TypeTag_Index]));
                MaterialData.RarityTag = FGameplayTag::RequestGameplayTag(FName(*RowValue[RarityTag_Index]));

                // AbilityTags CSV에서 분리 (예: "Tag1;Tag2;Tag3")
                TArray<FString> AbilityStrList;
                RowValue[AbilityTags_Index].ParseIntoArray(AbilityStrList, TEXT(";"), true);
                for (const FString& TagStr : AbilityStrList)
                {
                    MaterialData.AbilityTags.Add(FGameplayTag::RequestGameplayTag(FName(*TagStr)));
                }

                MaterialData.DisplayName = FText::FromString(RowValue[DisplayName_Index]);
                MaterialData.Description = FText::FromString(RowValue[Description_Index]);

                FString IconString = RowValue[Icon_Index];
                MaterialData.ItemIcon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(IconString));

                MaterialData.BasePrice = ParseIntValue(RowValue[BasePrice_Index]);
                MaterialData.StackMax = ParseIntValue(RowValue[StackMax_Index]);

                FName RowName = *FString::Printf(TEXT("Material_%d"), i);
                if (FMaterialItemDataStruct* FindRow = OutDataTable->FindRow<FMaterialItemDataStruct>(RowName, TEXT("")))
                {
                    *FindRow = MaterialData;
                }
                else
                {
                    OutDataTable->AddRow(RowName, MaterialData);
                }
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
