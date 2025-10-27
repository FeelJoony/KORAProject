// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "CSVToDataTableToolSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCSVToDataTableTool, Log, All);

UCLASS()
class KORACUSTOMEDITORS_API UCSVToDataTableToolSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(CallInEditor, Category = "CSV to DataTable Tool")
	void ConvertAllCSVsToDataTables();

private:
	bool ProcessCSVFile(const FString& InFullCSVFilePath, const FString& InDataTableName, const FString& InDataTablePackagePath, const FString& InStructName);

	UScriptStruct* FindStructByName(const FString& StructName);

	bool ReadCSVFileWithSharedAccess(const FString& FilePath, FString& OutString);

};

struct FConverter_Params
{
	UDataTable* DataTable;
	FString CSVString;
};
