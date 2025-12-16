#include "DataTool/TableRowConvertFunctionContainer.h"
#include "UObject/SoftObjectPath.h"
#include "UObject/SoftObjectPtr.h"
#include "Engine/Texture2D.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "SEditorViewportToolBarMenu.h"

#include "Data/ConsumeDataStruct.h"
#include "Data/CurrencyDataStruct.h"
#include "Data/QuestDataStruct.h"
#include "Data/SubQuestDataStruct.h"
#include "Data/ItemDataStruct.h"
#include "Data/TutorialDataStruct.h"
#include "Data/WeaponEnhanceDataStruct.h"
#include "Data/ShopItemDataStruct.h"
#include "Data/EquipAbilityDataStruct.h"
#include "Data/EquipDataStruct.h"
#include "Data/ModuleDataStruct.h"

struct FConsumeDataStruct;

UTableRowConvertFunctionContainer::UTableRowConvertFunctionContainer()
{

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
                int32 FragmentTags_Index = GetHeaderIndex(Headers, TEXT("FragmentTags"));
                int32 BasePrice_Index = GetHeaderIndex(Headers, TEXT("BasePrice"));
                int32 DisplayNameKey_Index = GetHeaderIndex(Headers, TEXT("DisplayNameKey"));
                int32 DescriptionKey_Index = GetHeaderIndex(Headers, TEXT("DescriptionKey"));
                int32 Icon_Index = GetHeaderIndex(Headers, TEXT("Icon"));
                int32 EquipID_Index = GetHeaderIndex(Headers, TEXT("EquipID"));
                int32 AbilityID_Index = GetHeaderIndex(Headers, TEXT("AbilityID"));
                int32 ConsumeID_Index = GetHeaderIndex(Headers, TEXT("ConsumeID"));
                int32 MaterialID_Index = GetHeaderIndex(Headers, TEXT("MaterialID"));
                int32 QuestID_Index = GetHeaderIndex(Headers, TEXT("QuestID"));
            	int32 ModuleID_Index = GetHeaderIndex(Headers, TEXT("ModuleID"));
                
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
            	ItemData.ModuleID = ParseIntValue(RowValue[ModuleID_Index]);

                TArray<FString> AbilityTagsStringValues = ParseArrayValue(RowValue[FragmentTags_Index]);
                for (const FString& StringValue : AbilityTagsStringValues)
                {
                    FGameplayTag AbilityTag = FGameplayTag::RequestGameplayTag(FName(StringValue));
                    ItemData.FragmentTags.Add(AbilityTag);
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
            	int32 Type_Index = GetHeaderIndex(Headers, TEXT("Type"));
                int32 QuestName_Index = GetHeaderIndex(Headers, TEXT("QuestName"));
                int32 Description_Index = GetHeaderIndex(Headers, TEXT("Description"));
                int32 StateTreeDefinitionAssetPath_Index = GetHeaderIndex(Headers, TEXT("StateTreeDefinitionAssetPath"));

                FQuestDataStruct QuestData;
                
                QuestData.Index = ParseIntValue(RowValue[Index_Index]);
            	QuestData.Type = ParseEnumValue<EQuestType>(RowValue[Type_Index]);
                QuestData.QuestName = RowValue[QuestName_Index];
                QuestData.Description = RowValue[Description_Index];
                QuestData.StateTreeDefinitionPath = ParseNameValue(RowValue[StateTreeDefinitionAssetPath_Index]);

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
            	int32 UIRowName_Index = GetHeaderIndex(Headers, TEXT("UIRowName"));
            	
                FSubQuestEvalDataStruct EvalData;

                EvalData.OrderIndex = ParseIntValue(RowValue[OrderIndex_Index]);
                EvalData.ObjectiveTag = ParseGameplayTagValue(RowValue[ObjectiveTag_Index]);
                EvalData.SubQuestName = ParseTextValue(RowValue[SubQuestName_Index]);
                EvalData.Description = ParseTextValue(RowValue[Description_Index]);
                EvalData.RequiredCount = ParseIntValue(RowValue[RequiredCount_Index]);
                EvalData.TimeLimit = ParseFloatValue(RowValue[TimeLimit_Index]);
            	EvalData.UIRowName = ParseNameValue(RowValue[UIRowName_Index]);
                
                if (TempIndex < ParseIntValue(RowValue[GroupIndex_Index]))
                {
                    FSubQuestDataStruct SubQuestData;
                    SubQuestData.GroupIndex = ParseIntValue(RowValue[GroupIndex_Index]);
                    
                    TempIndex = SubQuestData.GroupIndex;
                    TempRowName = *FString::Printf(TEXT("SubQuest_%d"), i);

                	SubQuestData.EvalDatas.Add(EvalData);
                	
                    FName RowName = TempRowName;
                    if (FSubQuestDataStruct* FindRow = OutDataTable->FindRow<FSubQuestDataStruct>(RowName, TEXT("")))
                    {
                        *FindRow = SubQuestData;
                    }
                    else
                    {
                        OutDataTable->AddRow(RowName, SubQuestData);
                    }
                }
                else if (TempIndex == ParseIntValue(RowValue[GroupIndex_Index]))
                {
                    if (FSubQuestDataStruct* FindRow = OutDataTable->FindRow<FSubQuestDataStruct>(TempRowName, TEXT("")))
                    {
                        FindRow->EvalDatas.Add(EvalData);
                    }
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

void UTableRowConvertFunctionContainer::CreateConsumeData(UDataTable* OutDataTable, const FString& InCSVString)
{
	CreateData(InCSVString, FString(TEXT("ConsumeData")), FParseMethod::CreateLambda(
		[&](FParseMethodParams Params)
		{
			auto& Headers = const_cast<TMap<FName, int32>&>(Params.Headers);
			auto& Values  = const_cast<TArray<TArray<FString>>&>(Params.Values);
			
			const int32 Index_Idx = GetHeaderIndex(Headers, TEXT("Index"));
			if (Index_Idx == INDEX_NONE)
			{
				return;
			}
			
			const int32 MainEffectClass_Idx     = GetHeaderIndex(Headers, TEXT("MainEffectClass"));
			const int32 EffectType_Idx          = GetHeaderIndex(Headers, TEXT("EffectType"));
			const int32 Power_Idx               = GetHeaderIndex(Headers, TEXT("Power"));
			const int32 Duration_Idx            = GetHeaderIndex(Headers, TEXT("Duration"));
			const int32 StackMax_Idx            = GetHeaderIndex(Headers, TEXT("StackMax"));
			const int32 CooldownDuration_Idx    = GetHeaderIndex(Headers, TEXT("CooldownDuration"));
			const int32 CooldownTag_Idx         = GetHeaderIndex(Headers, TEXT("CooldownTag"));
			const int32 InUseTags_Idx           = GetHeaderIndex(Headers, TEXT("InUseTags"));
			
			auto ParseEffectType = [](const FString& InStr) -> EConsumableEffectType
			{
				const FString Trimmed = InStr.TrimStartAndEnd();

				if (Trimmed.IsEmpty())
				{
					return EConsumableEffectType::Instant;
				}
				
				if (UEnum* Enum = StaticEnum<EConsumableEffectType>())
				{
					int64 Value = Enum->GetValueByNameString(Trimmed);
					if (Value != INDEX_NONE)
					{
						return static_cast<EConsumableEffectType>(Value);
					}
				}
				
				if (Trimmed.Equals(TEXT("Instant"), ESearchCase::IgnoreCase))
				{
					return EConsumableEffectType::Instant;
				}
				if (Trimmed.Equals(TEXT("HasDuration"), ESearchCase::IgnoreCase) ||
					Trimmed.Equals(TEXT("Has Duration"), ESearchCase::IgnoreCase))
				{
					return EConsumableEffectType::HasDuration;
				}
				if (Trimmed.Equals(TEXT("Infinite"), ESearchCase::IgnoreCase))
				{
					return EConsumableEffectType::Infinite;
				}

				return EConsumableEffectType::Instant;
			};
			
			auto ParseGEClass = [](const FString& InStr) -> TSoftClassPtr<UGameplayEffect>
			{
				TSoftClassPtr<UGameplayEffect> Result;
				const FString Trimmed = InStr.TrimStartAndEnd();
				if (!Trimmed.IsEmpty())
				{
					Result = TSoftClassPtr<UGameplayEffect>(FSoftObjectPath(Trimmed));
				}
				return Result;
			};

			for (int32 i = 0; i < Values.Num(); ++i)
			{
				TArray<FString>& RowValue = Values[i];
				
				if (!RowValue.IsValidIndex(Index_Idx))
				{
					continue;
				}

				FConsumeDataStruct ConsumeData;
				
				ConsumeData.Index = ParseIntValue(RowValue[Index_Idx]);
				if (ConsumeData.Index < 0)
				{
					continue;
				}

				if (MainEffectClass_Idx != INDEX_NONE && RowValue.IsValidIndex(MainEffectClass_Idx))
				{
					ConsumeData.MainEffectClass = ParseGEClass(RowValue[MainEffectClass_Idx]);
				}
				
				if (EffectType_Idx != INDEX_NONE && RowValue.IsValidIndex(EffectType_Idx))
				{
					ConsumeData.EffectType = ParseEffectType(RowValue[EffectType_Idx]);
				}
				else
				{
					ConsumeData.EffectType = EConsumableEffectType::Instant;
				}
				
				if (Power_Idx != INDEX_NONE && RowValue.IsValidIndex(Power_Idx))
				{
					ConsumeData.Power = ParseFloatValue(RowValue[Power_Idx]);
				}
				else
				{
					ConsumeData.Power = 0.f;
				}
				
				if (Duration_Idx != INDEX_NONE && RowValue.IsValidIndex(Duration_Idx))
				{
					ConsumeData.Duration = ParseFloatValue(RowValue[Duration_Idx]);
				}
				else
				{
					ConsumeData.Duration = 0.f;
				}
				
				if (CooldownDuration_Idx != INDEX_NONE && RowValue.IsValidIndex(CooldownDuration_Idx))
				{
					ConsumeData.CooldownDuration = ParseFloatValue(RowValue[CooldownDuration_Idx]);
				}
				else
				{
					ConsumeData.CooldownDuration = 0.f;
				}
				
				if (CooldownTag_Idx != INDEX_NONE && RowValue.IsValidIndex(CooldownTag_Idx))
				{
					const FString TagStr = RowValue[CooldownTag_Idx].TrimStartAndEnd();

					if (!TagStr.IsEmpty())
					{
						ConsumeData.CooldownTag = ParseGameplayTagValue(TagStr);
					}
				}
				
				if (InUseTags_Idx != INDEX_NONE && RowValue.IsValidIndex(InUseTags_Idx))
				{
					const FString TagsStr = RowValue[InUseTags_Idx].TrimStartAndEnd();
					if (!TagsStr.IsEmpty())
					{
						TArray<FString> TagStrings;

						if (TagsStr.Contains(TEXT("|")))
						{
							TagsStr.ParseIntoArray(TagStrings, TEXT("|"), true);
						}
						else
						{
							TagsStr.ParseIntoArray(TagStrings, TEXT(","), true);
						}

						for (const FString& TagToken : TagStrings)
						{
							const FString TrimmedTag = TagToken.TrimStartAndEnd();
							if (!TrimmedTag.IsEmpty() && TrimmedTag != TEXT("-1"))
							{
								const FGameplayTag Tag = ParseGameplayTagValue(TrimmedTag);

								if (Tag.IsValid())
								{
									ConsumeData.InUseTags.AddTag(Tag);
								}
							}
						}
					}
				}

				if (StackMax_Idx != INDEX_NONE && RowValue.IsValidIndex(StackMax_Idx))
				{
					ConsumeData.StackMax = ParseIntValue(RowValue[StackMax_Idx]);
					if (ConsumeData.StackMax <= 0)
					{
						ConsumeData.StackMax = 1;
					}
				}
				else
				{
					ConsumeData.StackMax = 1;
				}
				
				const FName RowName = *FString::Printf(TEXT("Consume_%d"), ConsumeData.Index);

				if (FConsumeDataStruct* FindRow =
					OutDataTable->FindRow<FConsumeDataStruct>(RowName, TEXT("CreateConsumeData")))
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

void UTableRowConvertFunctionContainer::CreateCurrencyData(class UDataTable* OutDataTable, const FString& InCSVString)
{
	CreateData(InCSVString, FString(TEXT("CurrencyData")), FParseMethod::CreateLambda([&](FParseMethodParams Params)
		{
			auto& Headers = const_cast<TMap<FName, int32>&>(Params.Headers);
			auto& Values = const_cast<TArray<TArray<FString>>&>(Params.Values);

			for (int32 i = 0; i < Values.Num(); i++)
			{
				TArray<FString>& RowValue = Values[i];

				int32 Index_Index = GetHeaderIndex(Headers, TEXT("Index"));
				int32 CurrencyID_Index = GetHeaderIndex(Headers, TEXT("CurrencyID"));
				int32 CurrencyTag_Index = GetHeaderIndex(Headers, TEXT("CurrencyTag"));
				int32 bLossRule_Index = GetHeaderIndex(Headers, TEXT("bLossRule"));

				FCurrencyDataStruct CurrencyData;

				CurrencyData.Index = ParseIntValue(RowValue[Index_Index]);
				CurrencyData.CurrencyID = ParseNameValue(RowValue[CurrencyID_Index]);
				CurrencyData.CurrencyTag = ParseGameplayTagValue(RowValue[CurrencyTag_Index]);
				CurrencyData.bLossRule = ParseBoolValue(RowValue[bLossRule_Index]);

				FName RowName = *FString::Printf(TEXT("Currency_%d"), i);
				if (FCurrencyDataStruct* FindRow = OutDataTable->FindRow<FCurrencyDataStruct>(RowName, TEXT("")))
				{
					*FindRow = CurrencyData;
				}
				else
				{
					OutDataTable->AddRow(RowName, CurrencyData);
				}
			}
		}));
}

void UTableRowConvertFunctionContainer::CreateEquipData(class UDataTable* OutDataTable, const FString& InCSVString)
{
	CreateData(InCSVString, FString(TEXT("EquipData")), FParseMethod::CreateLambda(
		[&](FParseMethodParams Params)
		{
			auto& Headers = Params.Headers;
			auto& Values = Params.Values;

			for (int32 i = 0; i < Values.Num(); i++)
			{
				TArray<FString>& RowValue = const_cast<TArray<FString>&>(Values[i]);

				int32 GroupID_Index = GetHeaderIndex(Headers, TEXT("GroupID"));
				int32 EquipItemTag_Index = GetHeaderIndex(Headers, TEXT("EquipItemTag"));
				int32 SlotTag_Index = GetHeaderIndex(Headers, TEXT("SlotTag"));
				int32 EquipmentMesh_Index = GetHeaderIndex(Headers, TEXT("EquipmentMesh"));
				int32 OverrideMaterials_Index = GetHeaderIndex(Headers, TEXT("OverrideMaterials"));
				int32 CompatibleModuleSlots_Index = GetHeaderIndex(Headers, TEXT("ModuleSlotTag"));
				int32 EquipAbilityID_Index = GetHeaderIndex(Headers, TEXT("EquipAbilityID"));

				FEquipDataStruct EquipData;

				EquipData.GroupID = ParseIntValue(RowValue[GroupID_Index]);
				EquipData.EquipItemTag = ParseGameplayTagValue(RowValue[EquipItemTag_Index]);
				EquipData.SlotTag = ParseGameplayTagValue(RowValue[SlotTag_Index]);
				EquipData.EquipmentMesh = ParseSoftObjectValue<UStaticMesh>(RowValue[EquipmentMesh_Index]);

				TArray<FString> MaterialValueStrings = ParseArrayValue(RowValue[OverrideMaterials_Index]);
				for (const FString& MaterialValueString : MaterialValueStrings)
				{
					if (MaterialValueString == TEXT("-1"))
					{
						break;
					}

					EquipData.OverrideMaterials.Add(ParseSoftObjectValue<UMaterialInterface>(MaterialValueString));
				}

				EquipData.ModuleSlotTag = ParseGameplayTagValue(RowValue[CompatibleModuleSlots_Index]);
				EquipData.EquipAbilityID = ParseIntValue(RowValue[EquipAbilityID_Index]);

				FName RowName = *FString::Printf(TEXT("Equip_%d"), i);
				if (FEquipDataStruct* FindRow = OutDataTable->FindRow<FEquipDataStruct>(RowName, TEXT("")))
				{
					*FindRow = EquipData;
				}
				else
				{
					OutDataTable->AddRow(RowName, EquipData);
				}
			}
		}));
}

void UTableRowConvertFunctionContainer::CreateEquipAbilityData(class UDataTable* OutDataTable, const FString& InCSVString)
{
	CreateData(InCSVString, FString(TEXT("EquipAbilityData")), FParseMethod::CreateLambda(
		[&](FParseMethodParams Params)
		{
			auto& Headers = Params.Headers;
			auto& Values = Params.Values;

			for (int32 i = 0; i < Values.Num(); i++)
			{
				TArray<FString>& RowValue = const_cast<TArray<FString>&>(Values[i]);

				int32 EquipAbilityID_Index = GetHeaderIndex(Headers, TEXT("EquipAbilityID"));
				int32 AbilityCount_Index = GetHeaderIndex(Headers, TEXT("AbilityCount"));

				FEquipAbilityDataStruct EquipAbilityData;
				EquipAbilityData.EquipAbilityID = ParseIntValue(RowValue[EquipAbilityID_Index]);
				EquipAbilityData.AbilityCount = ParseIntValue(RowValue[AbilityCount_Index]);
				EquipAbilityData.AbilityModifiers.Reset();

				const int32 Count = FMath::Clamp(EquipAbilityData.AbilityCount, 0, 9);

				for (int32 j = 0; j < Count; j++)
				{
					int32 AbilityeTypeTag_Index = GetHeaderIndex(Headers, *FString::Printf(TEXT("AbilityTypeTag%d"), j + 1));
					int32 BaseValue_Index = GetHeaderIndex(Headers, *FString::Printf(TEXT("BaseValue%d"), j + 1));
					int32 IncValue_Index = GetHeaderIndex(Headers, *FString::Printf(TEXT("IncValue%d"), j + 1));

					const FString TagStr = RowValue[AbilityeTypeTag_Index].TrimStartAndEnd();
					if (TagStr.IsEmpty() || TagStr == TEXT("-1")) continue;

					FKREquipAbilityModifierRow EquipAbilityModifierRow;
					EquipAbilityModifierRow.AbilityTypeTag = ParseGameplayTagValue(RowValue[AbilityeTypeTag_Index]);
					EquipAbilityModifierRow.BaseValue = ParseFloatValue(RowValue[BaseValue_Index]);
					EquipAbilityModifierRow.IncValue = ParseFloatValue(RowValue[IncValue_Index]);

					if (EquipAbilityModifierRow.AbilityTypeTag.IsValid())
					{
						EquipAbilityData.AbilityModifiers.Add(EquipAbilityModifierRow);
					}
				}
				
				FName RowName = *FString::Printf(TEXT("EquipAbility_%d"), i);
				if (FEquipAbilityDataStruct* FindRow = OutDataTable->FindRow<FEquipAbilityDataStruct>(RowName, TEXT("")))
				{
					*FindRow = EquipAbilityData;
				}
				else
				{
					OutDataTable->AddRow(RowName, EquipAbilityData);
				}
			}
		}));
}

void UTableRowConvertFunctionContainer::CreateModuleData(class UDataTable* OutDataTable, const FString& InCSVString)
{	CreateData(InCSVString, FString(TEXT("ModuleData")), FParseMethod::CreateLambda(
		[&](FParseMethodParams Params)
		{
			auto& Headers = Params.Headers;
			auto& Values = Params.Values;

			for (int32 i = 0; i < Values.Num(); i++)
			{
				TArray<FString>& RowValue = const_cast<TArray<FString>&>(Values[i]);

				int32 GroupID_Index = GetHeaderIndex(Headers, TEXT("GroupID"));
				int32 ModuleItemTag_Index = GetHeaderIndex(Headers, TEXT("ModuleItemTag"));
				int32 ModuleSlotTag_Index = GetHeaderIndex(Headers, TEXT("SlotTag"));
				int32 EquipAbilityID_Index = GetHeaderIndex(Headers, TEXT("EquipAbilityID"));


				FModuleDataStruct ModuleData;;
				ModuleData.GroupID = ParseIntValue(RowValue[GroupID_Index]);
				ModuleData.ModuleItemTag = ParseGameplayTagValue(RowValue[ModuleItemTag_Index]);
				ModuleData.SlotTag = ParseGameplayTagValue(RowValue[ModuleSlotTag_Index]);
				ModuleData.EquipAbilityID = ParseIntValue(RowValue[EquipAbilityID_Index]);
				
				FName RowName = *FString::Printf(TEXT("Module_%d"), i);
				if (FModuleDataStruct* FindRow = OutDataTable->FindRow<FModuleDataStruct>(RowName, TEXT("")))
				{
					*FindRow = ModuleData;
				}
				else
				{
					OutDataTable->AddRow(RowName, ModuleData);
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
