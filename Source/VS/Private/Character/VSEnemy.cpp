// LAvid


#include "Character/VSEnemy.h"
#include "AbilitySystemComponent.h"
#include "VSGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Subsystem/VSEnemyManager.h"
#include "Player/VS_PlayerState.h"


class AVS_PlayerState;

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
	TimeSinceLastAttack = 0.f;
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
	
	TimeSinceLastAttack = 0.f;
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
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Damage: [%f]"), DamageToApply));
	
	Health = FMath::Max(0.f, Health - DamageToApply);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Health: [%f]"), Health));
	
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
			 UAbilitySystemComponent* TargetASC = PS->GetAbilitySystemComponent();
			
			if (TargetASC)
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



void AVSEnemy::RefreshDefaultMaxHealth()
{
	MaxHealth = DefaultMaxHealth *(bIsElite ? EliteHealthMultiplier : 1.f);
}



