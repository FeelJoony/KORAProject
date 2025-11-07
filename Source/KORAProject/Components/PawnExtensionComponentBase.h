#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PawnExtensionComponentBase.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KORAPROJECT_API UPawnExtensionComponentBase : public UActorComponent
{
	GENERATED_BODY()

protected:
	template<class T>
	T* GetOwningPawn() const;

	template<class T>
	T* GetOwningController() const;
	
	APawn* GetOwningPawn() const { return GetOwningPawn<APawn>(); }
	
};

template <class T>
T* UPawnExtensionComponentBase::GetOwningPawn() const
{
	static_assert(TPointerIsConvertibleFromTo<T, APawn>::Value, "'T' Template Parameter to GetPawn must be derived from APawn");
	return CastChecked<T>(GetOwner());
}

template <class T>
T* UPawnExtensionComponentBase::GetOwningController() const
{
	static_assert(TPointerIsConvertibleFromTo<T, AController>::Value, "'T' Template Parameter to GetController must be derived from AController");
	return GetOwningPawn<APawn>()->GetController<T>();
}
