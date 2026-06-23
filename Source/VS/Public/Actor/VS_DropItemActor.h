// LAvid

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/Subsystem/VSDropData.h"
#include "VS_DropItemActor.generated.h"

class UStaticMeshComponent;

UCLASS()
class VS_API AVS_DropItemActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AVS_DropItemActor();

public:
	FORCEINLINE EVSDropType GetDropType() const { return DropType; }
	FORCEINLINE float GetDropValue() const { return DropValue; }
	FORCEINLINE bool IsActive() const { return bIsActive; }
	FORCEINLINE UStaticMeshComponent* GetDropMeshComponent() const { return DropMeshComponent; }
	
	void ActivateDrop(EVSDropType InDropType, float InValue);
	void DeActivateDrop();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Drop")
	EVSDropType DropType = EVSDropType::None;
	
	UPROPERTY(VisibleAnywhere, Category = "Drop")
	float DropValue = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Drop")
	TObjectPtr<UStaticMeshComponent> DropMeshComponent;
	
	bool bIsActive = false;
};
