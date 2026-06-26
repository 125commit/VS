// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/VSProjectileType.h"
#include "GameplayTagContainer.h"
#include "VS_WeaponActor.generated.h"

class USphereComponent;
class UNiagaraComponent;
class AVSEnemy;



/** Ability 传给 Actor 的运行时参数包 */
USTRUCT(BlueprintType)
struct FVSWeaponInitParams
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Category = "VS|Weapon")
	float Area = 1.f;
	
	UPROPERTY(BlueprintReadOnly, Category = "VS|Weapon")
	float Duration = 0.f;
	
	UPROPERTY(BlueprintReadOnly, Category = "VS|Weapon")
	float Damage = 0.f;
	
	UPROPERTY(BlueprintReadOnly, Category = "VS|Weapon")
	FRotator FacingRotation = FRotator::ZeroRotator;
	
	// for projectiles
	UPROPERTY(BlueprintReadOnly, Category = "VS|Weapon")
	float ProjectileSpeed = 600.f;
	
	// 速度倍率（1.0 = 100%）：圣经按级缩放转速、FireWand 按级缩放飞行速度
	UPROPERTY(BlueprintReadOnly, Category = "VS|Weapon")
	float SpeedMultiplier = 1.f;
	
	// NOTE: 可额外穿透的敌人数；<0 表示无限穿透。Straight/Homing 投射物均使用
	UPROPERTY(BlueprintReadOnly, Category = "VS|Weapon")
	int32 PierceCount = 0;
	
	/** Projectile的飞行模式：由 GA 在 spawn 前设置 */
	UPROPERTY(BlueprintReadOnly, Category = "VS|Weapon")
	EVSProjectileMovementMode MovementMode = EVSProjectileMovementMode::Straight;
	
	/** 用于 Straight 模式：已归一化的水平飞行方向 */
	UPROPERTY(BlueprintReadOnly, Category = "VS|Weapon")
	FVector FlightDirection = FVector::ForwardVector;
	
	UPROPERTY(BlueprintReadOnly, Category = "VS|Weapon")
	TWeakObjectPtr<AVSEnemy> TargetEnemy;

	// 击退力度(cm/s)：由 Ability 按最终伤害预先算好后下发；0 = 不击退
	UPROPERTY(BlueprintReadOnly, Category = "VS|Weapon")
	float KnockbackForce = 0.f;
	
	UPROPERTY(BlueprintReadOnly, Category = "VS|Weapon")
	float KnockbackDuration = 0.15f;
	
	// ICD：同一武器再次命中同一敌人的最小间隔(秒)
	UPROPERTY(BlueprintReadOnly, Category = "VS|Weapon")
	float HitInterval = 0.2f;
	
	// 武器身份（= Ability 的 AbilityTag），用作 ICD 分组键
	UPROPERTY(BlueprintReadOnly, Category = "VS|Weapon")
	FGameplayTag WeaponTag;

};


/**
 * 
 */
UCLASS()
class VS_API AVS_WeaponActor : public AActor
{
	GENERATED_BODY()
	
public:
	AVS_WeaponActor();
	
	virtual void InitWeapon(float InArea, float InDuration, float InDamage);
	virtual void InitFromParams(const FVSWeaponInitParams& InitParams);
	
	virtual void ActivateWeapon(const FVSWeaponInitParams& InitParams, AActor* InOwner, APawn* InInstigator);
	virtual void DeactivateWeapon();
	
protected:
	
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	virtual void SweepInitialOverlaps();
	
	FTimerHandle LifetimeTimerHandle;
	void OnLifetimeEnd();
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USphereComponent> SphereCollision;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraComponent> WeaponEffect;
	
	
	float WeaponDamage = 0.f;
	
	// ———— 击退————
	float KnockbackForce = 0.f;
	float KnockbackDuration = 0.15f;
	float HitInterval = 0.2f;
	FGameplayTag WeaponTag;

};
