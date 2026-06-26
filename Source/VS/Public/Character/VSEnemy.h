// LAvid

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Character/VSCharacterBase.h"
#include "VSEnemy.generated.h"

class UDataTable;
class UGameplayEffect;
struct FVSEnemyDefinition;

UCLASS()
class VS_API AVSEnemy : public AVSCharacterBase
{
    GENERATED_BODY()

public:
    AVSEnemy();

    // 出池时一次性灌注全部数据（取代 ResetForSpawn + SetIsElite + RefreshDefaultMaxHealth）
    void ApplyDefinition(const FVSEnemyDefinition& Def, bool bInElite);

    FORCEINLINE bool GetIsElite() const { return bIsElite; }
    FORCEINLINE bool IsDead() const { return bIsDead; }
    FORCEINLINE UDataTable* GetDropTable() const { return DropTable; }

    // 设置“移动”速度（驱动动画）
    void SetVisualSpeed(float Speed);

    // 回池前清理
    void OnRecycled();

    // 接受玩家攻击
    virtual float TakeDamage(float Damage, const FDamageEvent& DamageEvent,
        AController* EventInstigator, AActor* DamageCauser) override;

    // EnemyManager 在攻击范围内每帧调用
    void AttackPlayer(AActor* PlayerActor, float DeltaTime);

    // ── 击退系统对外接口 ──
    void ApplyKnockback(const FVector& SourceLocation, float BaseForce, float Duration);   //处理数据
    bool CanBeHitByWeapon(const FGameplayTag& WeaponTag, float HitInterval);               //判断能否被击退
    bool TickKnockback(float DeltaTime);                                                   //实现击退位移
    
    FORCEINLINE bool IsKnockedBack() const { return bIsKnockedBack; }

    // 仅大蒜调用：临时削减击退抗性。Amount=削减量(0~1)，Duration=持续秒数
    void ApplyKnockbackResistanceReduction(float Amount, float Duration);
    // 当前有效抗性 = Clamp(基础 - 当前生效的削减, 0, 1)
    float GetEffectiveKnockbackResistance() const;
    
    
    // 运行时数值（由 ApplyDefinition 灌注；不再在 BP 编辑）
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "State")
    float VisualSpeed = 200.f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "State")
    float MoveSpeed = 200.f;

protected:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attributes")
    float Health = 30.f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attributes")
    float MaxHealth = 30.f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attributes")
    bool bIsElite = false;

    // 接触伤害数值（运行时缓存）
    float AttackDamage = 5.f;
    float AttackInterval = 1.f;

    UPROPERTY()
    TSubclassOf<UGameplayEffect> DamageEffectClass;

    UPROPERTY()
    TObjectPtr<UDataTable> DropTable;

    // 击退抗性 0~1（由定义灌注）
    float KnockbackResistance = 0.f;

private:
    bool bIsDead = false;
    float TimeSinceLastAttack = 0.f;

    // 击退运行时状态（线性衰减）
    bool    bIsKnockedBack     = false;
    FVector KnockbackDir       = FVector::ZeroVector;
    float   KnockbackInitSpeed = 0.f;
    float   KnockbackDuration  = 0.f;
    float   KnockbackTimeLeft  = 0.f;
    
    // 抗性削减 debuff
    float KnockbackResistanceReduction  = 0.f;
    //debuff过期时间（世界时间）
    float ResistanceReductionExpireTime = 0.f;

    // ICD：WeaponID -> 上次命中本敌人的世界时间(秒)
    TMap<FGameplayTag, float> LastHitTimeByWeapon;

    // 抗性 ≥ 该值 → 完全免疫
    static constexpr float FullImmunityThreshold = 0.95f;
    
   
};