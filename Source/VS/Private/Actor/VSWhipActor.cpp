// LAvid


#include "Actor/VSWhipActor.h"

#include "Character/VSEnemy.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"


AVSWhipActor::AVSWhipActor()
{
	if (SphereCollision)
	{
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SphereCollision->SetSphereRadius(1.f);
	}
	
	WhipCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("WhipCollision"));
	WhipCollision->SetupAttachment(SphereCollision);
	WhipCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WhipCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	WhipCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AVSWhipActor::InitFromParams(const FVSWeaponInitParams& InitParams)
{
	SetActorRotation(InitParams.FacingRotation);
	SetWhipCollision();
	Super::InitFromParams(InitParams);
}

void AVSWhipActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() && WhipCollision)
	{
		WhipCollision->OnComponentBeginOverlap.AddDynamic(this, &AVSWhipActor::OnOverlapBegin);
	}
	
	// 已经在范围内的Actor不会触发OnOverlapBegin，所以要补判
	if (HasAuthority() && WhipCollision)
	{
		TArray<AActor*> Overlapping;
		WhipCollision->GetOverlappingActors(Overlapping, AVSEnemy::StaticClass());
		for (AActor* Actor : Overlapping)
		{
			OnOverlapBegin(WhipCollision, Actor, nullptr, INDEX_NONE, false, FHitResult());
		}
	}
}

void AVSWhipActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SetWhipCollision();
}

void AVSWhipActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AVSWhipActor::SetWhipCollision()
{
	if (!WhipCollision) return;
	WhipCollision->SetBoxExtent(BaseBoxExtent);
	WhipCollision->SetRelativeLocation(BaseBoxOffset);
}

