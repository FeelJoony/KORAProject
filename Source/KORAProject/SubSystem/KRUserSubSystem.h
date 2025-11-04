#pragma once

#include "CoreMinimal.h"
#include "Contents/KRContents.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "KRUserSubSystem.generated.h"

UCLASS()
class KORAPROJECT_API UKRUserSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;


private:
	UPROPERTY(Transient)
	TMap<TSubclassOf<UKRContents>, TObjectPtr<UKRContents>> Contents;

// Setter Section
private:
	template<typename TContents>
	void AddContents()
	{
		static_assert(TIsDerivedFrom<TContents, UKRContents>::IsDerived, "Template type TContents must be derived from UKRContents");

		TContents* NewContents = NewObject<TContents>(this);
		NewContents->Initialize();

		Contents.Add(TContents::StaticClass(), NewContents);
	}

	template<typename TContents>
	void RemoveContents()
	{
		static_assert(TIsDerivedFrom<TContents, UKRContents>::IsDerived, "Template type TContents must be derived from UKRContents");

		UKRContents* KRBaseContents = *Contents.Find(TContents::StaticClass());
		TContents* RemoveContents = CastChecked<TContents>(KRBaseContents);

		RemoveContents->DeInitialize();
	}

// Getter Section
public:
	template<typename TContents>
	FORCEINLINE TContents* GetContents() const
	{
		static_assert(TIsDerivedFrom<TContents, UKRContents>::IsDerived, "Template type TContents must be derived from UKRContents");

		UKRContents* KRBaseContent = *Contents.Find(TContents::StaticClass()).Get();
		TContents* KRContent = CastChecked<TContents>(KRBaseContent);

		return KRContent;
	}

};
