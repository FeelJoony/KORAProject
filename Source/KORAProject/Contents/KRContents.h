#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "KRContents.generated.h"

UCLASS()
class KORAPROJECT_API UKRContents : public UObject
{
	GENERATED_BODY()

public:
	UKRContents();

	virtual void Initialize();
	virtual void DeInitialize();

	// TODO : 아레에 두 함수는 후에 매개변수는 Json을 받아서 일괄 Parsing이 아닌 개별 Parsing 하게끔 각 Contents에서 가상 함수로 구현.
	virtual void SerializeUserData();
	virtual void DeserializeUserData();

	// TODO : 후에 추가될 UserData 매개변수를 받도록 구현
	virtual void SetUserData();
};
