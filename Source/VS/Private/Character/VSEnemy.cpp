// LAvid


#include "Character/VSEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Subsystem/VSEnemyManager.h"


AVSEnemy::AVSEnemy()
{
	PrimaryActorTick.bCanEverTick = false;

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->bAutoActivate = false;
		MovementComponent->Deactivate();
	}
	
	RefreshDefaultMaxHealth();
	Health = MaxHealth;
}

void AVSEnemy::SetIsElite(bool bIsInElite)
{
	bIsElite = bIsInElite;
	RefreshDefaultMaxHealth();
	if (!bIsDead)
	{
		Health = MaxHealth;
	}

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
}

//从池中拿出来之前重置状态
void AVSEnemy::ResetForSpawn()
{
	bIsDead = false;
	bIsElite = false;
	SetVisualSpeed(0.f);
	RefreshDefaultMaxHealth();
	Health = MaxHealth;
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
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

void AVSEnemy::RefreshDefaultMaxHealth()
{
	MaxHealth = DefaultMaxHealth *(bIsElite ? EliteHealthMultiplier : 1.f);
}



