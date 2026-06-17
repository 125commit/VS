// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/VS_WeaponActor.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/VSEnemy.h"

AVS_WeaponActor::AVS_WeaponActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// 1. 创建碰撞体作为根节点
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	SetRootComponent(CollisionSphere);
	
	// 碰撞预设：默认只触发 Overlap
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// 2. 创建 Niagara 特效并附着在碰撞体上
	WeaponEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("WeaponEffect"));
	WeaponEffect->SetupAttachment(RootComponent);
}

void AVS_WeaponActor::BeginPlay()
{
	Super::BeginPlay();
	
	// 仅在服务端绑定重叠事件，保证伤害判定的权威性
	if (HasAuthority())
	{
		CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AVS_WeaponActor::OnOverlapBegin);
	}
}

void AVS_WeaponActor::InitWeapon(float InArea, float InDuration, float InDamage)
{
	WeaponDamage = InDamage;

	// 1. 缩放范围 (Area)
	// 由于特效附着在碰撞体上，缩放 Root 会让两者一起同步缩放
	SetActorScale3D(FVector(InArea));

	// 2. 生命倒计时 (Duration)
	if (InDuration > 0.f)
	{
		SetLifeSpan(InDuration); // 引擎自带自毁函数
	}
}

void AVS_WeaponActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 排除自身和发射者(主角)
	if (!OtherActor || OtherActor == this || OtherActor == GetInstigator()) return;

	// 过滤：只对 VSEnemy 造成伤害
	if (Cast<AVSEnemy>(OtherActor))
	{
		if (WeaponDamage > 0.f)
		{
			// 不再使用 GE，直接使用内置 ApplyDamage
			UGameplayStatics::ApplyDamage(
				OtherActor, 
				WeaponDamage, 
				GetInstigatorController(), 
				this, 
				UDamageType::StaticClass()
			);
		}
	}
}
