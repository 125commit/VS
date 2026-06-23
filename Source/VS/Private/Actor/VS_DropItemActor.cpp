// LAvid

#include "Actor/VS_DropItemActor.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AVS_DropItemActor::AVS_DropItemActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	DropMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("DropMeshComponent");
	SetRootComponent(DropMeshComponent);
	DropMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DropMeshComponent->SetGenerateOverlapEvents(false);
	DropMeshComponent->SetCanEverAffectNavigation(false);
	SetActorHiddenInGame(true);
}

void AVS_DropItemActor::ActivateDrop(EVSDropType InDropType, float InValue)
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

void AVS_DropItemActor::DeActivateDrop()
{
	DropType = EVSDropType::None;
	DropValue = 0.f;
	bIsActive = false;
	SetActorHiddenInGame(true);
}
