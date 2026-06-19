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
	
	RefreshDefaultScale();
	//用BodyScale恢复正常体型
}

void AVSEnemy::SetIsElite(bool bIsInElite)
{
	bIsElite = bIsInElite;
	RefreshDefaultMaxHealth();
	if (!bIsDead)
	{
		Health = MaxHealth;
	}
	//增大体型
}

void AVSEnemy::SetVisualSpeed(float InSpeed)
{
	VisualSpeed = InSpeed;
}

//再次从对象池中拿出来循环用
void AVSEnemy::OnRecycled()
{
	bIsDead = false;
	SetVisualSpeed(0.f);
	RefreshDefaultMaxHealth();
	Health = MaxHealth;
}

void AVSEnemy::ResetForSpawn()
{
	bIsDead = false;
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
	
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	const float DamageToApply = ActualDamage > 0.f ? ActualDamage : Damage;
	
	Health = FMath::Max(0.f, Health - DamageToApply);
	
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
		if (UWorld* World = GetWorld())
		{
			if (UVSEnemyManager* EnemyManager = World->GetSubsystem<UVSEnemyManager>())
			{
				EnemyManager->OnEnemyDie(this);
			}
		}
	}
	return DamageToApply;
	
	
	
}

void AVSEnemy::RefreshDefaultMaxHealth()
{
	MaxHealth = DefaultMaxHealth *(bIsElite ? EliteHealthMultiplier : 1.f);
}

void AVSEnemy::RefreshDefaultScale()
{
	BodyScale = bIsElite ? 1.25f : 1.f;
}

