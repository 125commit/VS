// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/VS_WeaponActor.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/VSEnemy.h"

AVS_WeaponActor::AVS_WeaponActor()
{
	SphereCollision = CreateDefaultSubobject<USphereComponent>("SphereCollisionComponent");
	SetRootComponent(SphereCollision);
	
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	WeaponEffect = CreateDefaultSubobject<UNiagaraComponent>("WeaponEffect");
	WeaponEffect->SetupAttachment(RootComponent);
}

void AVS_WeaponActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (!HasAuthority()) return;
	
	if (SphereCollision)
	{
		SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AVS_WeaponActor::OnOverlapBegin);
	}
	
	// 敌人在鞭击生成前已站在范围内时，OverlapBegin 不会触发，需补判
	SweepInitialOverlaps();
}

void AVS_WeaponActor::SweepInitialOverlaps()
{
	if (!SphereCollision) return;
	
	TArray<AActor*> OverlappingActors;
	SphereCollision->GetOverlappingActors(OverlappingActors);
	
	for (AActor* OverlappingActor : OverlappingActors)
	{
		OnOverlapBegin(SphereCollision, OverlappingActor, nullptr, INDEX_NONE, false, FHitResult());
	}
	
}


void AVS_WeaponActor::InitWeapon(float InArea, float InDuration, float InDamage)
{
	FVSWeaponInitParams WeaponInitParams;
	WeaponInitParams.Area = InArea;
	WeaponInitParams.Duration = InDuration;
	WeaponInitParams.Damage = InDamage;
	
	InitFromParams(WeaponInitParams);
	
}

void AVS_WeaponActor::InitFromParams(const FVSWeaponInitParams& InitParams)
{
	WeaponDamage = InitParams.Damage;

	SetActorScale3D(FVector(InitParams.Area));
	
	if (InitParams.Duration > 0.f)
	{
		SetLifeSpan(InitParams.Duration);
	}
	else
	{
		SetLifeSpan(0.15f);
	}
}

void AVS_WeaponActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority() || !OtherActor || OtherActor == this || OtherActor == GetInstigator()) return;
	
	AVSEnemy* Enemy = Cast<AVSEnemy>(OtherActor);
	if (!Enemy || Enemy->IsDead() || WeaponDamage < 0.f) return;
	
	UGameplayStatics::ApplyDamage(
		Enemy,
		WeaponDamage,
		GetInstigatorController(),
		this,
		UDamageType::StaticClass()
	);
}