// LAvid


#include "Character/VSEnemy.h"
#include "AbilitySystemComponent.h"
#include "VSGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Subsystem/VSEnemyManager.h"
#include "Player/VS_PlayerState.h"
#include "Data/Subsystem/DA_EnemyDictionary.h"


class AVS_PlayerState;

AVSEnemy::AVSEnemy()
{
	PrimaryActorTick.bCanEverTick = false;

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->bAutoActivate = false;
		MovementComponent->Deactivate();
	}

	Health = MaxHealth;
}

void AVSEnemy::ApplyDefinition(const FVSEnemyDefinition& Def, bool bInElite)
{
	// 1) 身份与血量
	bIsDead  = false;
	bIsElite = bInElite;
	const float HpMult = bIsElite ? Def.EliteHealthMultiplier : 1.f;
	MaxHealth = Def.MaxHealth * HpMult;
	Health    = MaxHealth;
	
	// 2) 行为数值
	MoveSpeed           = Def.MoveSpeed;
	AttackDamage        = Def.AttackDamage;
	AttackInterval      = Def.AttackInterval;
	DamageEffectClass   = Def.DamageEffectClass;
	DropTable           = Def.DropTable;
	KnockbackResistance = FMath::Clamp(Def.KnockbackResistance, 0.f, 1.f);
	// 3) 体型（精英放大）。注意：外壳 BP 的根缩放需保持 1，避免二次缩放
	const float FinalScale = Def.BodyScale * (bIsElite ? Def.EliteScaleMultiplier : 1.f);
	SetActorScale3D(FVector(FinalScale));
	// 4) 运行时状态重置（合并原 ResetForSpawn）
	SetVisualSpeed(0.f);
	TimeSinceLastAttack = 0.f;
	bIsKnockedBack    = false;
	KnockbackTimeLeft = 0.f;
	LastHitTimeByWeapon.Reset();
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
}


void AVSEnemy::SetVisualSpeed(float InSpeed)
{
	VisualSpeed = InSpeed;
}

//回池前的清理
void AVSEnemy::OnRecycled()
{
	SetVisualSpeed(0.f);
	bIsElite = false;
	TimeSinceLastAttack = 0.f;
	
	bIsKnockedBack    = false;
	KnockbackTimeLeft = 0.f;
	LastHitTimeByWeapon.Reset();
}


float AVSEnemy::TakeDamage(float Damage, const FDamageEvent& DamageEvent, AController* EventInstigator,
                           AActor* DamageCauser)
{
	if (!HasAuthority() || bIsDead || Damage <= 0.f)
	{
		return 0.f;
	}
	
	//保证已死亡的敌人不会再接受伤害
	if (const UVSEnemyManager* Manager = GetWorld()->GetSubsystem<UVSEnemyManager>())
	{
		if (!Manager->ActiveEnemies.Contains(this)) return 0.f;
	}
	
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	const float DamageToApply = ActualDamage > 0.f ? ActualDamage : Damage;
	
	Health = FMath::Max(0.f, Health - DamageToApply);
	
	UVSEnemyManager* EnemyManager = GetWorld()->GetSubsystem<UVSEnemyManager>();
	
	// STEP: 广播"被命中"（吸血鞭据此回血）
	if (EnemyManager)
	{
		EnemyManager->OnEnemyDamagedDelegate.Broadcast(this, DamageToApply, DamageCauser);
	}
	
	if (Health <= 0.f)
	{
		if (bIsDead)
		{
			return 0.f;
		}
		bIsDead = true;
		
		// 防止继续被 Overlap / 移动逻辑命中
		SetActorEnableCollision(false);
		SetVisualSpeed(0.f);
		
		if (EnemyManager)
		{
			EnemyManager->OnEnemyDie(this, DamageCauser);
		}
	}
	return DamageToApply;
}

void AVSEnemy::AttackPlayer(AActor* PlayerActor, float DeltaTime)
{
	if (!HasAuthority() || bIsDead || !PlayerActor || !DamageEffectClass) return;
	
	TimeSinceLastAttack += DeltaTime;
	if (TimeSinceLastAttack < AttackInterval) return;
	TimeSinceLastAttack = 0.f;
	
	
	if (const APawn* Pawn = Cast<APawn>(PlayerActor))
	{
		if (const AVS_PlayerState* PS = Pawn->GetPlayerState<AVS_PlayerState>())
		{
			if (UAbilitySystemComponent* TargetASC = PS->GetAbilitySystemComponent())
			{
				FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
				ContextHandle.AddSourceObject(this);
				FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(DamageEffectClass, 1.f, ContextHandle);
	
				SpecHandle.Data->SetSetByCallerMagnitude(FVSGameplayTags::Get().Data_Damage, AttackDamage);
	
				TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
	
	
	
	
}

// 击退系统 ————————
void AVSEnemy::ApplyKnockback(const FVector& SourceLocation, float BaseForce, float Duration)
{
	if (bIsDead || BaseForce <= 0.f || Duration <= 0.f) return;
	
	const float Resist = GetEffectiveKnockbackResistance();
	
	// 完全免疫档：高抗性/Boss（抗性 > 0.95） 只掉血，既不被击退也不硬直
	if (Resist >= FullImmunityThreshold)
	{
		return;
	}
	
	// 抗性公式：实际力度 = 基础力度 * (1 - 抗性)
	const float ActualForce = BaseForce * (1.f - Resist);
	
	// 计算方向
	FVector Dir = GetActorLocation() - SourceLocation;
	Dir.Z = 0.f;
	
	if (Dir.IsNearlyZero())
	{
		Dir = -GetActorForwardVector();
		Dir.Z = 0.f;
	}
	
	Dir = Dir.GetSafeNormal();
	
	// 状态刷新（非叠加）
	KnockbackDir       = Dir;
	KnockbackInitSpeed = ActualForce;
	KnockbackDuration  = Duration;
	KnockbackTimeLeft  = Duration;
	bIsKnockedBack     = true;
}

bool AVSEnemy::CanBeHitByWeapon(const FGameplayTag& WeaponTag, float HitInterval)
{
	const float NowSeconds = GetWorld()->GetTimeSeconds();
	
	// "Find" 返回的是地址，所以值要用指针
	if (const float* LastHitTime = LastHitTimeByWeapon.Find(WeaponTag))
	{
		if (NowSeconds - *LastHitTime < HitInterval)
		{
			return false; // 仍在 ICD 内 → 跳过
		}
	}
	// 记录被某个武器击中的时间
	LastHitTimeByWeapon.Add(WeaponTag, NowSeconds);
	return true;
}

bool AVSEnemy::TickKnockback(float DeltaTime)
{
	if (!bIsKnockedBack) return false;
	
	//计算被击退的剩余时间
	KnockbackTimeLeft -= DeltaTime;
	if (KnockbackTimeLeft <= 0.f)
	{
		bIsKnockedBack = false;
		return false;
	}
	
	// ***给击退速度一个衰减效果
	const float Alpha = KnockbackTimeLeft / KnockbackDuration; // 1 → 0
	const float Speed = KnockbackInitSpeed * Alpha;
	
	//碰撞检测（可能撞墙） & 击退位移
	FHitResult Hit;
	SetActorLocation(GetActorLocation() + KnockbackDir * Speed * DeltaTime, /*bSweep=*/true, &Hit);
	
	// ***击退位移被阻止（撞墙），改变击退方向，使其沿墙滑动
	if (Hit.bBlockingHit)
	{
		KnockbackDir = FVector::VectorPlaneProject(KnockbackDir, Hit.Normal).GetSafeNormal();
	}
	SetVisualSpeed(0.f);
	return true;
}

void AVSEnemy::ApplyKnockbackResistanceReduction(float Amount, float Duration)
{
	if (Amount <= 0.f || Duration <= 0.f) return;
	const float Now = GetWorld()->GetTimeSeconds();
	
	// 上一次削减已过期则重置； 仍生效则取较强者，最后统一刷新持续时间（光环持续覆盖）
	if (Now > ResistanceReductionExpireTime)
	{
		KnockbackResistanceReduction = Amount;
	}
	
	else
	{
		KnockbackResistanceReduction = FMath::Max(KnockbackResistanceReduction, Amount);
	}
	
	ResistanceReductionExpireTime = Now + Duration;
}

float AVSEnemy::GetEffectiveKnockbackResistance() const
{
	float Effective = KnockbackResistance;
	if (GetWorld() && GetWorld()->GetTimeSeconds() <= ResistanceReductionExpireTime)
	{
		//计算有效的击退抗性
		Effective -= KnockbackResistanceReduction;
	}
	return FMath::Clamp(Effective, 0.f, 1.f);
}





