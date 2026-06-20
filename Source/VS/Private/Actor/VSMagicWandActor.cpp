// LAvid


#include "Actor/VSMagicWandActor.h"

#include "NiagaraComponent.h"
#include "Character/VSEnemy.h"
#include "Components/SphereComponent.h"
#include "Subsystem/VSWeaponSubsysem.h"


AVSMagicWandActor::AVSMagicWandActor()
{
	PrimaryActorTick.bCanEverTick = true;

	if (SphereCollision)
	{
		SphereCollision->SetSphereRadius(DefaultSphereRadius);
	}
}

void AVSMagicWandActor::InitFromParams(const FVSWeaponInitParams& InitParams)
{
	ProjectileSpeed = InitParams.ProjectileSpeed;
	TargetEnemy = InitParams.TargetEnemy;
	bHasHit = false;
	
	Super::InitFromParams(InitParams);
}

void AVSMagicWandActor::SetupInstigatorIgnore(APawn* InstigatorPawn)
{
	//忽略玩家(Instigator)
	if (!SphereCollision || !InstigatorPawn) return;
	SphereCollision->IgnoreActorWhenMoving(InstigatorPawn, true);
	SphereCollision->MoveIgnoreActors.AddUnique(InstigatorPawn);
}

void AVSMagicWandActor::EnableProjectileCollision()
{
	if (!HasAuthority() || bHasHit) return;
	
	SetActorEnableCollision(true);
	
	if (SphereCollision)
	{
		SphereCollision->SetGenerateOverlapEvents(true);
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void AVSMagicWandActor::ActivateWeapon(const FVSWeaponInitParams& InitParams, AActor* InOwner, APawn* InInstigator)
{
	if (!HasAuthority()) return;
	
	InitFromParams(InitParams);
	
	SetOwner(InOwner);
	SetInstigator(InInstigator);
	SetActorHiddenInGame(false);
	
	//先关闭碰撞
	if (SphereCollision)
	{
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SphereCollision->SetGenerateOverlapEvents(false);
	}
	SetActorEnableCollision(false);
	
	// 忽略玩家
	SetupInstigatorIgnore(InInstigator);
	
	//先移动一帧再恢复 Actor 的碰撞：第一帧 Tick 会飞出 SpawnForwardOffset 方向
	SetActorEnableCollision(true);
	
	
	if (WeaponEffect)
	{
		WeaponEffect->Activate(true);
	}
	
	
	//恢复碰撞
	GetWorldTimerManager().ClearTimer(CollisionEnableTimerHandle);
	GetWorldTimerManager().SetTimer(
		CollisionEnableTimerHandle,
		this,
		&AVSMagicWandActor::EnableProjectileCollision,
		CollisionEnableDelay,
		false
	);
	
	 //生命周期，到时间强制回收
	const float Duration = InitParams.Duration > 0.f ? InitParams.Duration : 3.f;
	GetWorldTimerManager().SetTimer(
		LifetimeTimerHandle,
		this,
		&AVSMagicWandActor::OnLifetimeEnd,
		Duration,
		false);
}

void AVSMagicWandActor::DeactivateWeapon()
{
	GetWorldTimerManager().ClearTimer(CollisionEnableTimerHandle);
	
	//关闭碰撞
	if (SphereCollision)
	{
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SphereCollision->SetGenerateOverlapEvents(false);
		SphereCollision->MoveIgnoreActors.Empty();
	}
	
	ProjectileSpeed = 0.f;
	TargetEnemy = nullptr;
	bHasHit = false;
	
	Super::DeactivateWeapon();
}

void AVSMagicWandActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority() && SphereCollision)
	{
		//TODO:优化基类
		SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AVSMagicWandActor::OnOverlapBegin);
	}
	
}

//自动索敌
void AVSMagicWandActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!HasAuthority() || bHasHit) return;
	
	if (!TargetEnemy.IsValid() || TargetEnemy->IsDead())
	{
		ReturnToPool();
		return;
	}
	
	const FVector CurrentLoc = GetActorLocation();
	const FVector TargetLoc = TargetEnemy->GetActorLocation();
	const FVector Direction = (TargetLoc - CurrentLoc).GetSafeNormal();
	
	if (Direction.IsNearlyZero())
	{
		ReturnToPool();
		return;
	}

	const FVector NewLoc = CurrentLoc + Direction * ProjectileSpeed * DeltaTime;
	SetActorLocation(NewLoc, true);
	
	
	const float HitRadius = SphereCollision ? SphereCollision->GetScaledSphereRadius() : DefaultSphereRadius;
	const float DistanceSq = FVector::DistSquared(GetActorLocation(), TargetEnemy->GetActorLocation());

	if (DistanceSq <= FMath::Square(HitRadius + 50.f)) // 50.f 可按敌人胶囊体半径调整
	{
		if (!bHasHit)
		{
			bHasHit = true;
			Super::OnOverlapBegin(SphereCollision, TargetEnemy.Get(), nullptr, INDEX_NONE, false, FHitResult());
			ReturnToPool();
		}
		return;
	}
	
	const FRotator NewRotation = FMath::RInterpTo(
	GetActorRotation(),
	Direction.Rotation(),
	DeltaTime,
	15.f
);
	SetActorRotation(NewRotation);
}

void AVSMagicWandActor::SweepInitialOverlaps()
{
	//不需要任何逻辑
}

void AVSMagicWandActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority() || bHasHit) return;
	if (!OtherActor || OtherActor == this || OtherActor == GetInstigator() || OtherActor == GetOwner()) return;
	
	AVSEnemy* Enemy = Cast<AVSEnemy>(OtherActor);
	if (!Enemy || Enemy->IsDead()) return;
	
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	bHasHit = true;
	ReturnToPool();
}

void AVSMagicWandActor::ReturnToPool()
{
	if (UWorld* World = GetWorld())
	{
		if (UVSWeaponSubsysem* WeaponSubsystem = World->GetSubsystem<UVSWeaponSubsysem>())
		{
			WeaponSubsystem->ReturnWeaponToPool(this);
		}
	}
}







