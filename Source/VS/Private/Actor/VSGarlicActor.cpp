// LAvid


#include "Actor/VSGarlicActor.h"

#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "Character/VSEnemy.h"
#include "Kismet/GameplayStatics.h"


AVSGarlicActor::AVSGarlicActor()
{
	if (SphereCollision)
	{
		SphereCollision->SetSphereRadius(BaseSphereRadius);
	}
}

void AVSGarlicActor::InitFromParams(const FVSWeaponInitParams& InitParams)
{
	// 不调用 Super：避免 SetLifeSpan(Duration) 和父类默认值
	WeaponDamage = InitParams.Damage;
	SetActorScale3D(FVector(InitParams.Area));
	
	// 大蒜不调 Super，自行缓存击退数据
	KnockbackForce    = InitParams.KnockbackForce;
	KnockbackDuration = InitParams.KnockbackDuration;
	HitInterval       = InitParams.HitInterval;
	WeaponTag         = InitParams.WeaponTag;
}

void AVSGarlicActor::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority() && SphereCollision)
	{
		SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AVSGarlicActor::OnOverlapBegin);
	}
}

void AVSGarlicActor::SweepInitialOverlaps()
{

}

void AVSGarlicActor::ActivateWeapon(const FVSWeaponInitParams& InitParams, AActor* InOwner, APawn* InInstigator)
{
	// 不调用 Super::ActivateWeapon：不设 LifetimeTimer、不回池
	
	if (!HasAuthority()) return;
	
	InitFromParams(InitParams);
	SetOwner(InOwner);
	SetInstigator(InInstigator);
	SetActorHiddenInGame(false);
	
	if (AActor* OwnerActor = GetOwner())
	{
		//附着在玩家身上
		AttachToActor(OwnerActor, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
	
	if (SphereCollision)
	{
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	SetActorEnableCollision(true);
	
	if (WeaponEffect)
	{
		WeaponEffect->Activate(true);
	}
	
}

void AVSGarlicActor::DeactivateWeapon()
{
	GetWorldTimerManager().ClearTimer(LifetimeTimerHandle);
	WeaponDamage = 0.f;
	SetActorScale3D(FVector::OneVector);
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	
	if (SphereCollision)
	{
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	SetActorEnableCollision(false);
	
	if (WeaponEffect)
	{
		WeaponEffect->Deactivate();
	}
	
	SetActorHiddenInGame(true);
}


//对已经在 Circle 内的 Actor 补上伤害
void AVSGarlicActor::ApplyPeriodicDamageInCircle(const FVSWeaponInitParams& InitParams)
{
	if (!HasAuthority()) return;
	
	WeaponDamage = InitParams.Damage;
	
	// 新增：每个 Cooldown 刷新击退/ICD（让击退随当前伤害同步增长）
	KnockbackForce    = InitParams.KnockbackForce;
	KnockbackDuration = InitParams.KnockbackDuration;
	HitInterval       = InitParams.HitInterval;
	WeaponTag         = InitParams.WeaponTag;
	
	// SetActorScale3D 会让已重叠的敌人再次触发 OnComponentBeginOverlap
	/** 所以需要 bCloseOverlapDamage 屏蔽 OnBeginOverlap 函数中的伤害，用当前函数来“补”伤害 **/
	bCloseOverlapDamage = true;
	SetActorScale3D(FVector(InitParams.Area));
	bCloseOverlapDamage = false;

	//Apply PassiveEffect & Damage
	if (!SphereCollision) return;
	
	TArray<AActor*> OverlappingActors;
	SphereCollision->GetOverlappingActors(OverlappingActors, AVSEnemy::StaticClass());
	
	for (AActor* Actor : OverlappingActors)
	{
		AVSEnemy* Enemy = Cast<AVSEnemy>(Actor);
		if (Enemy && !Enemy->IsDead())
		{
			if (!Enemy->CanBeHitByWeapon(WeaponTag, HitInterval)) continue;
			//先破甲
			ApplyGarlicPassiveEffects(Enemy);  
			
			//伤害
			UGameplayStatics::ApplyDamage(Enemy, WeaponDamage, GetInstigatorController(), this, UDamageType::StaticClass());
			
			// 击退（读到破甲后的有效抗性）
			if (KnockbackForce > 0.f)          
			{
				Enemy->ApplyKnockback(GetActorLocation(), KnockbackForce, KnockbackDuration);
			}
		}
	}
	
}


//对刚进入碰撞体的敌人造成伤害
void AVSGarlicActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bCloseOverlapDamage) return;
	if (!HasAuthority() || !OtherActor || OtherActor == this || OtherActor == GetInstigator()) return;
	AVSEnemy* Enemy = Cast<AVSEnemy>(OtherActor);
	if (!Enemy || Enemy->IsDead() || WeaponDamage <= 0.f) return;
	
	if (!Enemy->CanBeHitByWeapon(WeaponTag, HitInterval)) return;
	
	//先破甲
	ApplyGarlicPassiveEffects(Enemy);
	
	//伤害
	UGameplayStatics::ApplyDamage(
		Enemy,
		WeaponDamage,
		GetInstigatorController(),
		this,
		UDamageType::StaticClass());
	
	//击退
	if (KnockbackDuration > 0.f)
	{
		Enemy->ApplyKnockback(GetActorLocation(), KnockbackForce, KnockbackDuration);
	}
}


void AVSGarlicActor::ApplyGarlicPassiveEffects(AVSEnemy* Enemy)
{
	if (!Enemy) return;
	
	// 临时降低该敌人击退抗性
	if (KnockbackResistanceReduction > 0.f)
	{
		Enemy->ApplyKnockbackResistanceReduction(KnockbackResistanceReduction, ResistanceReductionDuration);
	}
}
