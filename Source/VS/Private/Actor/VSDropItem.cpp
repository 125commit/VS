// LAvid


#include "Actor/VSDropItem.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AVSDropItem::AVSDropItem()
{
	PrimaryActorTick.bCanEverTick = false;
	
	DropMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("DropMeshComponent");
	SetRootComponent(DropMeshComponent);
	DropMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DropMeshComponent->SetGenerateOverlapEvents(false);
	DropMeshComponent->SetCanEverAffectNavigation(false);
	SetActorHiddenInGame(true);
}

void AVSDropItem::ActivateDrop(EVSDropType InDropType, float InValue)
{
	if (InDropType == EVSDropType::None)
	{
		DeActivateDrop();
		return;
	}
	
	DropType = InDropType;
	DropValue = FMath::Max(0.f, InValue);
	bIsActive = true;
	
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	
	if (DropMeshComponent)
	{
		DropMeshComponent->SetVisibility(true);
		DropMeshComponent->SetHiddenInGame(false);
		DropMeshComponent->MarkRenderStateDirty();
	}
}

void AVSDropItem::DeActivateDrop()
{
	DropType = EVSDropType::None;
	DropValue = 0.f;
	bIsActive = false;
	SetActorHiddenInGame(true);
	
}





