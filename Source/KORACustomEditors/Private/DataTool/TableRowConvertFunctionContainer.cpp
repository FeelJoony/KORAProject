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
#include "Data/QuestDataStruct.h"
#include "Data/SubQuestDataStruct.h"
#include "Data/ItemDataStruct.h"

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
                
                for (int32 j = 1; j <= MaxElementCount_Index; j++)
                {

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

void UTableRowConvertFunctionContainer::CreateItemData(class UDataTable* OutDataTable, const FString& InCSVString)
{
    CreateData(InCSVString, FString(TEXT("ItemData")), FParseMethod::CreateLambda([&](FParseMethodParams Params)
        {
            auto& Headers = const_cast<TMap<FName, int32>&>(Params.Headers);
            auto& Values = const_cast<TArray<TArray<FString>>&>(Params.Values);

            for (int32 i = 0; i < Values.Num(); i++)
            {
                TArray<FString>& RowValue = Values[i];

                int32 Index_Index = GetHeaderIndex(Headers, TEXT("Index"));
                int32 TypeTag_Index = GetHeaderIndex(Headers, TEXT("TypeTag"));
                int32 RarityTag_Index = GetHeaderIndex(Headers, TEXT("RarityTag"));
                int32 AbilityTags_Index = GetHeaderIndex(Headers, TEXT("AbilityTags"));
                int32 BasePrice_Index = GetHeaderIndex(Headers, TEXT("BasePrice"));
                int32 DisplayNameKey_Index = GetHeaderIndex(Headers, TEXT("DisplayNameKey"));
                int32 DescriptionKey_Index = GetHeaderIndex(Headers, TEXT("DescriptionKey"));
                int32 Icon_Index = GetHeaderIndex(Headers, TEXT("Icon"));
                int32 EquipID_Index = GetHeaderIndex(Headers, TEXT("EquipID"));
                int32 AbilityID_Index = GetHeaderIndex(Headers, TEXT("AbilityID"));
                int32 ConsumeID_Index = GetHeaderIndex(Headers, TEXT("ConsumeID"));
                int32 MaterialID_Index = GetHeaderIndex(Headers, TEXT("MaterialID"));
                int32 QuestID_Index = GetHeaderIndex(Headers, TEXT("QuestID"));
                
                FItemDataStruct ItemData;

                ItemData.Index = ParseIntValue(RowValue[Index_Index]);
                ItemData.TypeTag = ParseGameplayTagValue(RowValue[TypeTag_Index]);
                ItemData.RarityTag = ParseGameplayTagValue(RowValue[RarityTag_Index]);
                ItemData.BasePrice = ParseIntValue(RowValue[BasePrice_Index]);
                ItemData.DisplayNameKey = ParseNameValue(RowValue[DisplayNameKey_Index]);
                ItemData.DescriptionKey = ParseNameValue(RowValue[DescriptionKey_Index]);
                ItemData.Icon = ParseSoftObjectValue<UTexture2D>(RowValue[Icon_Index]);
                ItemData.EquipID = ParseIntValue(RowValue[EquipID_Index]);
                ItemData.AbilityID = ParseIntValue(RowValue[AbilityID_Index]);
                ItemData.ConsumeID = ParseIntValue(RowValue[ConsumeID_Index]);
                ItemData.MaterialID = ParseIntValue(RowValue[MaterialID_Index]);
                ItemData.QuestID = ParseIntValue(RowValue[QuestID_Index]);

                TArray<FString> AbilityTagsStringValues = ParseArrayValue(RowValue[AbilityTags_Index]);
                for (const FString& StringValue : AbilityTagsStringValues)
                {
                    FGameplayTag AbilityTag = FGameplayTag::RequestGameplayTag(FName(StringValue));
                    ItemData.AbilityTags.Add(AbilityTag);
                }

                FName RowName = *FString::Printf(TEXT("Item_%d"), i);
                if (FItemDataStruct* FindRow = OutDataTable->FindRow<FItemDataStruct>(RowName, TEXT("")))
                {
                    *FindRow = ItemData;
                }
                else
                {
                    OutDataTable->AddRow(RowName, ItemData);
                }
            }
        }));
}

void UTableRowConvertFunctionContainer::CreateQuestData(class UDataTable* OutDataTable, const FString& InCSVString)
{
    CreateData(InCSVString, FString(TEXT("QuestData")), FParseMethod::CreateLambda([&](FParseMethodParams Params)
        {
            auto& Headers = const_cast<TMap<FName, int32>&>(Params.Headers);
            auto& Values = const_cast<TArray<TArray<FString>>&>(Params.Values);

            for (int32 i = 0; i < Values.Num(); i++)
            {
                TArray<FString>& RowValue = Values[i];

                int32 Index_Index = GetHeaderIndex(Headers, TEXT("Index"));
                int32 QuestName_Index = GetHeaderIndex(Headers, TEXT("QuestName"));
                int32 Description_Index = GetHeaderIndex(Headers, TEXT("Description"));
                int32 StateTreeDefinitionAssetName_Index = GetHeaderIndex(Headers, TEXT("StateTreeDefinitionAssetName"));

                FQuestDataStruct QuestData;
                
                QuestData.Index = ParseIntValue(RowValue[Index_Index]);
                QuestData.QuestName = RowValue[QuestName_Index];
                QuestData.Description = RowValue[Description_Index];
                QuestData.StateTreeDefinitionName = ParseNameValue(RowValue[StateTreeDefinitionAssetName_Index]);

                FName RowName = *FString::Printf(TEXT("Quest_%d"), i);
                if (FQuestDataStruct* FindRow = OutDataTable->FindRow<FQuestDataStruct>(RowName, TEXT("")))
                {
                    *FindRow = QuestData;
                }
                else
                {
                    OutDataTable->AddRow(RowName, QuestData);
                }
            }
        }));
}

void UTableRowConvertFunctionContainer::CreateSubQuestData(class UDataTable* OutDataTable, const FString& InCSVString)
{
    CreateData(InCSVString, FString(TEXT("SubQuestData")), FParseMethod::CreateLambda([&](FParseMethodParams Params)
        {
            auto& Headers = const_cast<TMap<FName, int32>&>(Params.Headers);
            auto& Values = const_cast<TArray<TArray<FString>>&>(Params.Values);

            int TempIndex = 0;
            FName TempRowName; 
            for (int32 i = 0; i < Values.Num(); i++)
            {
                TArray<FString>& RowValue = Values[i];

                int32 GroupIndex_Index = GetHeaderIndex(Headers, TEXT("GroupIndex"));
                int32 OrderIndex_Index = GetHeaderIndex(Headers, TEXT("OrderIndex"));
                int32 ObjectiveTag_Index = GetHeaderIndex(Headers, TEXT("ObjectiveTag"));
                int32 SubQuestName_Index = GetHeaderIndex(Headers, TEXT("SubQuestName"));
                int32 Description_Index = GetHeaderIndex(Headers, TEXT("Description"));
                int32 RequiredCount_Index = GetHeaderIndex(Headers, TEXT("RequiredCount"));
                int32 TimeLimit_Index = GetHeaderIndex(Headers, TEXT("TimeLimit"));

                FSubQuestEvalDataStruct EvalData;

                EvalData.OrderIndex = ParseIntValue(RowValue[OrderIndex_Index]);
                //EvalData.ObjectiveTag = ParseGameplayTagValue(RowValue[ObjectiveTag_Index]);
                EvalData.SubQuestName = ParseTextValue(RowValue[SubQuestName_Index]);
                EvalData.Description = ParseTextValue(RowValue[Description_Index]);
                EvalData.RequiredCount = ParseIntValue(RowValue[RequiredCount_Index]);
                EvalData.TimeLimit = ParseFloatValue(RowValue[TimeLimit_Index]);
                
                if (TempIndex < ParseIntValue(RowValue[GroupIndex_Index]))
                {
                    FSubQuestDataStruct SubQuestData;
                    SubQuestData.GroupIndex = ParseIntValue(RowValue[GroupIndex_Index]);
                    
                    TempIndex = SubQuestData.GroupIndex;
                    TempRowName = *FString::Printf(TEXT("SubQuest_%d"), i);

                    FName RowName = *FString::Printf(TEXT("SubQuest_%d"), i);
                    if (FSubQuestDataStruct* FindRow = OutDataTable->FindRow<FSubQuestDataStruct>(RowName, TEXT("")))
                    {
                        *FindRow = SubQuestData;
                    }
                    else
                    {
                        OutDataTable->AddRow(RowName, SubQuestData);
                    }

                    SubQuestData.EvalDatas.Add(EvalData);
                }
                else if (TempIndex == GroupIndex_Index)
                {
                    if (FSubQuestDataStruct* FindRow = OutDataTable->FindRow<FSubQuestDataStruct>(TempRowName, TEXT("")))
                    {
                        FindRow->EvalDatas.Add(EvalData);
                    }
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
