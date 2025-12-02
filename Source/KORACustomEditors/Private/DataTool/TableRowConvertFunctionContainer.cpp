#include "DataTool/TableRowConvertFunctionContainer.h"
#include "UObject/SoftObjectPath.h"
#include "UObject/SoftObjectPtr.h"
#include "Engine/Texture2D.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "Data/SampleDataStruct.h"
#include "Data/WeaponDataStruct.h"
#include "Data/ItemDataStruct.h"
#include "Data/TutorialDataStruct.h"
#include "Data/WeaponEnhanceDataStruct.h"
#include "Data/ShopItemDataStruct.h"

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
                ItemData.BasePrice = ParseFloatValue(RowValue[BasePrice_Index]);
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

void UTableRowConvertFunctionContainer::CreateWeaponData(class UDataTable* OutDataTable, const FString& InCSVString)
{
    CreateData(InCSVString, FString(TEXT("WeaponData")), FParseMethod::CreateLambda([&](FParseMethodParams Params)
        {
            auto& Headers = const_cast<TMap<FName, int32>&>(Params.Headers);
            auto& Values = const_cast<TArray<TArray<FString>>&>(Params.Values);

            for (int32 i = 0; i < Values.Num(); i++)
            {
                TArray<FString>& RowValue = Values[i];

                int32 GroupID_Index = GetHeaderIndex(Headers, TEXT("GroupID"));
                int32 Atk_Index = GetHeaderIndex(Headers, TEXT("Atk"));
                int32 CritChance_Index = GetHeaderIndex(Headers, TEXT("CritChance"));
                int32 CritMulti_Index = GetHeaderIndex(Headers, TEXT("CritMulti"));
                int32 Range_Index = GetHeaderIndex(Headers, TEXT("Range"));
                int32 Accuracy_Index = GetHeaderIndex(Headers, TEXT("Accuracy"));
                int32 Recoil_Index = GetHeaderIndex(Headers, TEXT("Recoil"));
                int32 Capacity_Index = GetHeaderIndex(Headers, TEXT("Capacity"));
                int32 ReloadTime_Index = GetHeaderIndex(Headers, TEXT("ReloadTime"));
                int32 AttackSpeed_Index = GetHeaderIndex(Headers, TEXT("AttackSpeed"));
                
                FWeaponDataStruct WeaponData;

                WeaponData.GroupID = ParseIntValue(RowValue[GroupID_Index]);
                WeaponData.Atk = ParseIntValue(RowValue[Atk_Index]);
                WeaponData.CritChance = ParseFloatValue(RowValue[CritChance_Index]);
                WeaponData.CritMulti = ParseFloatValue(RowValue[CritMulti_Index]);
                WeaponData.Range = ParseIntValue(RowValue[Range_Index]);
                WeaponData.Accuracy = ParseIntValue(RowValue[Accuracy_Index]);
                WeaponData.Recoil = ParseIntValue(RowValue[Recoil_Index]);
                WeaponData.Capacity = ParseIntValue(RowValue[Capacity_Index]);
                WeaponData.ReloadTime = ParseFloatValue(RowValue[ReloadTime_Index]);
                WeaponData.AttackSpeed = ParseFloatValue(RowValue[AttackSpeed_Index]);
                
                FName RowName = *FString::Printf(TEXT("Weapon_%d"), i);
                if (FWeaponDataStruct* FindRow = OutDataTable->FindRow<FWeaponDataStruct>(RowName, TEXT("")))
                {
                    *FindRow = WeaponData;
                }
                else
                {
                    OutDataTable->AddRow(RowName, WeaponData);
                }
            }
        }));
}

void UTableRowConvertFunctionContainer::CreateWeaponEnhanceData(class UDataTable* OutDataTable,
    const FString& InCSVString)
{
    CreateData(InCSVString, FString(TEXT("WeaponEnhanceData")), FParseMethod::CreateLambda([&](FParseMethodParams Params)
        {
            auto& Headers = const_cast<TMap<FName, int32>&>(Params.Headers);
            auto& Values = const_cast<TArray<TArray<FString>>&>(Params.Values);

            for (int32 i = 0; i < Values.Num(); i++)
            {
                TArray<FString>& RowValue = Values[i];

                int32 GroupID_Index = GetHeaderIndex(Headers, TEXT("GroupID"));
                int32 Lv_Index = GetHeaderIndex(Headers, TEXT("Lv"));
                int32 AddAtk_Index = GetHeaderIndex(Headers, TEXT("AddAtk"));
                int32 AddCritChance_Index = GetHeaderIndex(Headers, TEXT("AddCritChance"));
                int32 AddRange_Index = GetHeaderIndex(Headers, TEXT("AddRange"));
                int32 Success_Index = GetHeaderIndex(Headers, TEXT("Success"));
                int32 Cost_Index = GetHeaderIndex(Headers, TEXT("Cost"));
                
                FWeaponEnhanceDataStruct WeaponEnhanceData;

                WeaponEnhanceData.GroupID = ParseIntValue(RowValue[GroupID_Index]);
                WeaponEnhanceData.Lv = ParseIntValue(RowValue[Lv_Index]);
                WeaponEnhanceData.AddAtk = ParseIntValue(RowValue[AddAtk_Index]);
                WeaponEnhanceData.AddCritChance = ParseFloatValue(RowValue[AddCritChance_Index]);
                WeaponEnhanceData.AddRange = ParseIntValue(RowValue[AddRange_Index]);
                WeaponEnhanceData.Success = ParseIntValue(RowValue[Success_Index]);
                WeaponEnhanceData.Cost = ParseIntValue(RowValue[Cost_Index]);
                
                FName RowName = *FString::Printf(TEXT("WeaponEnhance_%d"), i);
                if (FWeaponEnhanceDataStruct* FindRow = OutDataTable->FindRow<FWeaponEnhanceDataStruct>(RowName, TEXT("")))
                {
                    *FindRow = WeaponEnhanceData;
                }
                else
                {
                    OutDataTable->AddRow(RowName, WeaponEnhanceData);
                }
            }
        }));
}

void UTableRowConvertFunctionContainer::CreateTutorialData(class UDataTable* OutDataTable, const FString& InCSVString)
{
    CreateData(InCSVString, FString(TEXT("TutorialData")), FParseMethod::CreateLambda([&](FParseMethodParams Params)
        {
            auto& Headers = const_cast<TMap<FName, int32>&>(Params.Headers);
            auto& Values = const_cast<TArray<TArray<FString>>&>(Params.Values);

            for (int32 i = 0; i < Values.Num()-1; i++)
            {
                TArray<FString>& RowValue = Values[i];

                int32 RowName_Index = GetHeaderIndex(Headers, TEXT("RowName"));
                int32 PopupType_Index = GetHeaderIndex(Headers, TEXT("PopupType"));
                int32 Duration_Index = GetHeaderIndex(Headers, TEXT("Duration"));
                int32 PauseGame_Index = GetHeaderIndex(Headers, TEXT("PauseGame"));
                int32 StringKey_Index = GetHeaderIndex(Headers, TEXT("StringKey"));
                int32 OffsetX_Index = GetHeaderIndex(Headers, TEXT("OffsetX"));
                int32 OffsetY_Index = GetHeaderIndex(Headers, TEXT("OffsetY"));
                int32 CompletionTrigger_Index = GetHeaderIndex(Headers, TEXT("CompletionTrigger"));
                
                FTutorialDataStruct TutorialData;
                
                TutorialData.PopupType = RowValue[PopupType_Index];
                TutorialData.Duration = ParseFloatValue(RowValue[Duration_Index]);
                TutorialData.PauseGame = ParseBoolValue(RowValue[PauseGame_Index]);
                TutorialData.StringKey = ParseNameValue(RowValue[StringKey_Index]);
                TutorialData.OffsetX = ParseFloatValue(RowValue[OffsetX_Index]);
                TutorialData.OffsetY = ParseFloatValue(RowValue[OffsetY_Index]);
                TutorialData.CompletionTrigger = ParseNameValue(RowValue[CompletionTrigger_Index]);

                
                FName RowName = ParseNameValue(RowValue[RowName_Index]);
                if (FTutorialDataStruct* FindRow = OutDataTable->FindRow<FTutorialDataStruct>(RowName, TEXT("")))
                {
                    *FindRow = TutorialData;
                }
                else
                {
                    OutDataTable->AddRow(RowName, TutorialData);
                }
            }
        }));
}

void UTableRowConvertFunctionContainer::CreateShopItemData(class UDataTable* OutDataTable, const FString& InCSVString)
{
    CreateData(InCSVString, FString(TEXT("ShopItemData")), FParseMethod::CreateLambda([&](FParseMethodParams Params)
        {
            auto& Headers = const_cast<TMap<FName, int32>&>(Params.Headers);
            auto& Values = const_cast<TArray<TArray<FString>>&>(Params.Values);

            for (int32 i = 0; i < Values.Num() - 1; i++)
            {
                TArray<FString>& RowValue = Values[i];

                int32 Index_Index = GetHeaderIndex(Headers, TEXT("Index"));
				int32 ItemTag_Index = GetHeaderIndex(Headers, TEXT("ItemTag"));
                int32 PoolTag_Index = GetHeaderIndex(Headers, TEXT("PoolTag"));
                int32 StockBase_Index = GetHeaderIndex(Headers, TEXT("StockBase"));
                int32 Weight_Index = GetHeaderIndex(Headers, TEXT("Weight"));

                FShopItemDataStruct ShopItemData;

                ShopItemData.Index = ParseIntValue(RowValue[Index_Index]);
				ShopItemData.ItemTag = ParseGameplayTagValue(RowValue[ItemTag_Index]);
                ShopItemData.PoolTag = ParseGameplayTagValue(RowValue[PoolTag_Index]);
                ShopItemData.StockBase = ParseIntValue(RowValue[StockBase_Index]);
                ShopItemData.Weight = ParseFloatValue(RowValue[Weight_Index]);

                FName RowName = *FString::Printf(TEXT("ShopItem_%d"), i);
                if (FShopItemDataStruct* FindRow = OutDataTable->FindRow<FShopItemDataStruct>(RowName, TEXT("")))
                {
                    *FindRow = ShopItemData;
                }
                else
                {
                    OutDataTable->AddRow(RowName, ShopItemData);
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
