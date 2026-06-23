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
	
	ProjectileSpeed = InitParams.ProjectileSpeed * FMath::Max(0.f, InitParams.SpeedMultiplier);
	TargetEnemy = InitParams.TargetEnemy;
	bStopOnHit = false;
	HitEnemies.Empty();
	
	MovementMode = InitParams.MovementMode;
	FlightDirection = InitParams.FlightDirection.GetSafeNormal(); //给FireWand用的飞行方向
	
	// NOTE: 每次出池都重置穿透上限，避免复用到上一发的残留值
	PierceCount = InitParams.PierceCount;
	
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
	PierceCount = 0;
	
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
	if (bStopOnHit) return;
	
	
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
		OnOverlapBegin(SphereCollision, TargetEnemy.Get(), nullptr, INDEX_NONE, false, FHitResult());
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
	// WARN: bStopOnHit 现在表示"穿透次数已耗尽、正在/已回池"，两种模式都用它做二次进入的防护
	if (bStopOnHit) return;
	if (!HasAuthority()) return;

	if (!OtherActor || OtherActor == this || OtherActor == GetInstigator() || OtherActor == GetOwner()) return;
	
	AVSEnemy* Enemy = Cast<AVSEnemy>(OtherActor);
	if (!Enemy || Enemy->IsDead()) return;
	
	// STEP: 同一颗投射物对同一敌人只结算一次伤害（穿透时尤为关键）
	if (HitEnemies.Contains(Enemy)) return;
	HitEnemies.Add(Enemy);
	
	// STEP: 结算伤害（基类负责 ApplyDamage）
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	// STEP: 判断穿透是否耗尽。PierceCount<0 视为无限穿透；否则命中数 > PierceCount 即回池
	// 默认 PierceCount=0：命中首个敌人后 HitEnemies.Num()==1 > 0 → 立即回池（FireWand 一打就消失）
	const bool bExhausted = (PierceCount >= 0 && HitEnemies.Num() > PierceCount);
	if (bExhausted)
	{
		bStopOnHit = true;
		ReturnToPool();
		return;
	}
	
	// STEP: 仍可继续穿透。Straight 直接保持直线飞行；Homing 必须重新索敌才能继续追击
	if (MovementMode == EVSProjectileMovementMode::Homing)
	{
		if (AVSEnemy* NextTarget = FindNextHomingTarget())
		{
			TargetEnemy = NextTarget;
		}
		else
		{
			// NOTE: 没有可追踪的新目标，提前回池（剩余穿透次数因场上无敌人而作废）
			bStopOnHit = true;
			ReturnToPool();
		}
	}
}

AVSEnemy* AVSProjectileActor::FindNextHomingTarget() const
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;
	
	UVSEnemyManager* EnemyManager = World->GetSubsystem<UVSEnemyManager>();
	if (!EnemyManager) return nullptr;
	
	const FVector MyLoc = GetActorLocation();
	AVSEnemy* Best = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();
	
	// STEP: 在存活且未被本颗命中过的敌人中选最近者
	for (AActor* Actor : EnemyManager->ActiveEnemies)
	{
		AVSEnemy* Enemy = Cast<AVSEnemy>(Actor);
		if (!Enemy || Enemy->IsDead()) continue;
		if (HitEnemies.Contains(Enemy)) continue;
		
		const float DistSq = FVector::DistSquared(MyLoc, Enemy->GetActorLocation());
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			Best = Enemy;
		}
	}
	
	return Best;
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






