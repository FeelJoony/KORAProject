//#pragma once
//
//#include "CoreMinimal.h"
//#include "Subsystems/GameInstanceSubsystem.h"
//#include "Engine/DataTable.h"
//#include "Data/GameDataType.h"
//#include "Data/BaseItemData.h"
//#include "Item/Object/KRBaseItem.h"
//#include "KRItemSubsystem.generated.h"
//
//UCLASS()
//class KORAPROJECT_API UKRItemSubsystem : public UGameInstanceSubsystem
//{
//	GENERATED_BODY()
//
//public:
//	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
//	virtual void Deinitialize() override;
//
//	const FBaseItemData* GetItemData(EGameDataType ItemType, int32 ItemKey) const;
//	
//	UFUNCTION(BlueprintCallable, Category = "ItemSubsystem")
//	UKRBaseItem* CreateItemInstance(EGameDataType ItemType, int32 ItemKey, int32 Amount = 1);
//
//protected:
//	UPROPERTY(Transient)
//	TMap<EGameDataType, TObjectPtr<UDataTable>> ItemDataTables;
//
//private:
//	const FString TablePath = TEXT("/Game/Data/DataTables");
//	
//	void LoadDataTable(EGameDataType ItemType, const FString& TableName);
//};