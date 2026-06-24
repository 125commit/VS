// LAvid

#pragma once

#include "CoreMinimal.h"
#include "Character/VSCharacterBase.h"
#include "VSEnemy.generated.h"

class UDataTable;
class UGameplayEffect;

/**
 * 
 */
UCLASS()
class VS_API AVSEnemy : public AVSCharacterBase
{
	GENERATED_BODY()
	
public:
	AVSEnemy();
	
	void SetIsElite(bool bIsInElite);
	FORCEINLINE bool GetIsElite() const {return bIsElite;}
	FORCEINLINE bool IsDead() const {return bIsDead;}
	
	FORCEINLINE UDataTable* GetDropTable() const { return DropTable; }
	
	
	//设置“移动”速度
	void SetVisualSpeed(float Speed);
	
	//回池前清理
	void OnRecycled();
	
	//从池中取出使用前，先重置状态
	void ResetForSpawn();
	
	// 接受玩家攻击
	virtual float TakeDamage(float Damage, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;
	
	// EnemyManager 在攻击范围内每帧调用，对玩家造成伤害
	void AttackPlayer(AActor* PlayerActor, float DeltaTime);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float VisualSpeed = 200.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float MoveSpeed = 200.f;

	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float DefaultMaxHealth = 30.f;
	
	// 精英相对普通怪的血量倍率
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float EliteHealthMultiplier = 2.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	float Health = 30.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	float MaxHealth = 30.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float BodyScale = 1.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop")
	TObjectPtr<UDataTable> DropTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	bool bIsElite = false;

	// 每次接触造成的伤害（每个敌人 BP 各自配置 → 天然“按类型不同”）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float AttackDamage = 5.f;
	
	// 两次攻击最小间隔，避免每帧扣血
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float AttackInterval = 1.f;
	
	// 接触伤害用的 GE（指向 GE_EnemyDamage）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

private:
	bool bIsDead = false;
	void RefreshDefaultMaxHealth();
	
	// 距离上一次攻击的时间
	float TimeSinceLastAttack = 0.f;

	
};
