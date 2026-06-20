// LAvid


#include "Actor/VSProjectileActor.h"

#include "NiagaraComponent.h"
#include "Character/VSEnemy.h"
#include "Components/SphereComponent.h"
#include "Subsystem/VSEnemyManager.h"
#include "Subsystem/VSWeaponSubsysem.h"


AVSProjectileActor::AVSProjectileActor()
{
	PrimaryActorTick.bCanEverTick = true;

	if (SphereCollision)
	{
		SphereCollision->SetSphereRadius(DefaultSphereRadius);
	}
}

void AVSProjectileActor::InitFromParams(const FVSWeaponInitParams& InitParams)
{
	ProjectileSpeed = InitParams.ProjectileSpeed;
	TargetEnemy = InitParams.TargetEnemy;
	bStopOnHit = false;
	HitEnemies.Empty();
	
	MovementMode = InitParams.MovementMode;
	FlightDirection = InitParams.FlightDirection.GetSafeNormal(); //给FireWand用的飞行方向
	
	Super::InitFromParams(InitParams);
}


void AVSProjectileActor::ActivateWeapon(const FVSWeaponInitParams& InitParams, AActor* InOwner, APawn* InInstigator)
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
	
	if (WeaponEffect)
	{
		WeaponEffect->Activate(true);
	}
	
	//恢复碰撞
	GetWorldTimerManager().ClearTimer(CollisionEnableTimerHandle);
	GetWorldTimerManager().SetTimer(
		CollisionEnableTimerHandle,
		this,
		&AVSProjectileActor::EnableProjectileCollision,
		CollisionEnableDelay,
		false
	);
	
	 //生命周期，到时间强制回收
	const float Duration = InitParams.Duration > 0.f ? InitParams.Duration : 3.f;

	GetWorldTimerManager().ClearTimer(LifetimeTimerHandle);
	GetWorldTimerManager().SetTimer(
		LifetimeTimerHandle,
		this,
		&AVSProjectileActor::OnLifetimeEnd,
		Duration,
		false);
}

void AVSProjectileActor::DeactivateWeapon()
{
	GetWorldTimerManager().ClearTimer(CollisionEnableTimerHandle);
	
	//关闭碰撞
	if (SphereCollision)
	{
		//忽略玩家
		if (APawn* InstigatorPawn = GetInstigator())
		{
			SphereCollision->IgnoreActorWhenMoving(InstigatorPawn, false);
		}
		
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SphereCollision->SetGenerateOverlapEvents(false);
	}
	
	ProjectileSpeed = 0.f;
	TargetEnemy = nullptr;
	bStopOnHit = false;
	HitEnemies.Empty();
	
	Super::DeactivateWeapon();
}

void AVSProjectileActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority() && SphereCollision)
	{
		SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AVSProjectileActor::OnOverlapBegin);
	}
	
}

//自动索敌
void AVSProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!HasAuthority()) return;
	if (MovementMode == EVSProjectileMovementMode::Homing && bStopOnHit) return;
	
	
	// ── FireWand：只沿固定方向飞，不索敌 ── //
	if (MovementMode == EVSProjectileMovementMode::Straight)
	{
		if (FlightDirection.IsNearlyZero())
		{
			ReturnToPool();
			return;
		}
		
		SetActorLocation(GetActorLocation() + FlightDirection * ProjectileSpeed * DeltaTime, true);
		SetActorRotation(FlightDirection.Rotation());
		return;  // 命中只靠 OnOverlapBegin
	}
	
	
	// ── MagicWand：你现有的追踪逻辑，原样保留 ── //
	
	//只追踪有效的敌人
	UVSEnemyManager* EnemyManager = GetWorld()->GetSubsystem<UVSEnemyManager>();
	if (!EnemyManager || !EnemyManager->ActiveEnemies.Contains(TargetEnemy.Get()))
	{
		ReturnToPool();
		return;
	}
	
	if (!TargetEnemy.IsValid() || TargetEnemy->IsDead())
	{
		ReturnToPool();
		return;
	}
	
	/* 防止“看上去击中了” ，没有碰撞回收 */
	const float HitRadius = SphereCollision ? SphereCollision->GetScaledSphereRadius() : DefaultSphereRadius;
	const float DistanceSq = FVector::DistSquared(GetActorLocation(), TargetEnemy->GetActorLocation());

	if (DistanceSq <= FMath::Square(HitRadius + 50.f)) // 50.f 可按敌人胶囊体半径调整
	{
		if (!bStopOnHit)
		{
			bStopOnHit = true;
			Super::OnOverlapBegin(SphereCollision, TargetEnemy.Get(), nullptr, INDEX_NONE, false, FHitResult());
			ReturnToPool();
		}
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
	
	const FRotator NewRotation = FMath::RInterpTo(
	GetActorRotation(),
	Direction.Rotation(),
	DeltaTime,
	15.f);
	
	SetActorRotation(NewRotation);
}


void AVSProjectileActor::SweepInitialOverlaps()
{
	//不需要任何逻辑
}

void AVSProjectileActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//只有MagicWand才是一碰撞就回池
	if (MovementMode == EVSProjectileMovementMode::Homing && bStopOnHit) return;
	if (!HasAuthority()) return;

	if (!OtherActor || OtherActor == this || OtherActor == GetInstigator() || OtherActor == GetOwner()) return;
	
	AVSEnemy* Enemy = Cast<AVSEnemy>(OtherActor);
	if (!Enemy || Enemy->IsDead()) return;
	
	
	// ── FireWand：穿透，同敌只伤一次，不回池 ──
	
	if (MovementMode == EVSProjectileMovementMode::Straight)
	{
		// 已伤害过该敌人，跳过
		for (const TWeakObjectPtr<AVSEnemy>& HitEnemy : HitEnemies)
		{
			if (HitEnemy.Get() == Enemy)
			{
				return;
			}
		}
		HitEnemies.Add(Enemy);
		Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
		return;
	}
	
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	bStopOnHit = true;
	ReturnToPool();
	
	
}

void AVSProjectileActor::ReturnToPool()
{
	if (UWorld* World = GetWorld())
	{
		if (UVSWeaponSubsysem* WeaponSubsystem = World->GetSubsystem<UVSWeaponSubsysem>())
		{
			WeaponSubsystem->ReturnWeaponToPool(this);
		}
	}
}

void AVSProjectileActor::SetupInstigatorIgnore(APawn* InstigatorPawn)
{
	if (!SphereCollision || !InstigatorPawn) return;
	//忽略玩家(Instigator)
	SphereCollision->IgnoreActorWhenMoving(InstigatorPawn, true);
}

void AVSProjectileActor::EnableProjectileCollision()
{
	if (!HasAuthority()) return;
	if (MovementMode == EVSProjectileMovementMode::Homing && bStopOnHit) return;
	
	SetActorEnableCollision(true);
	
	if (SphereCollision)
	{
		SphereCollision->SetGenerateOverlapEvents(true);
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}






