// LAvid

#pragma once

#include "CoreMinimal.h"
#include "Actor/VS_WeaponActor.h"
#include "VSProjectileActor.generated.h"

UCLASS()
class VS_API AVSProjectileActor : public AVS_WeaponActor
{
	GENERATED_BODY()
	
public:	
	AVSProjectileActor();

protected:
	virtual void InitFromParams(const FVSWeaponInitParams& InitParams) override;
	virtual void ActivateWeapon(const FVSWeaponInitParams& InitParams, AActor* InOwner, APawn* InInstigator) override;
	virtual void DeactivateWeapon() override;
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	virtual void SweepInitialOverlaps() override;
	
	virtual void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;
	
	void ReturnToPool();
	
	/**
	 * 职责：在剩余穿透次数内，为 Homing 投射物挑选下一个最近且未被本颗命中过的敌人
	 * 原因：Homing 命中目标后不再立即回池，需要一个新目标继续追踪，否则会停在原目标处空转
	 * 返回：找不到可追踪的新目标时返回 nullptr，调用方据此回池
	 */
	AVSEnemy* FindNextHomingTarget() const;
	
	/* 用于碰撞延迟 */ 
	void SetupInstigatorIgnore(APawn* InstigatorPawn);
	void EnableProjectileCollision();
	
	UPROPERTY(EditDefaultsOnly, Category = "VS|MagicWand")
	float CollisionEnableDelay = 0.05f;
	
	FTimerHandle CollisionEnableTimerHandle;
	/* */
	
	UPROPERTY(EditDefaultsOnly, Category = "VS|MagicWand")
	float DefaultSphereRadius = 15.f;
	
	float ProjectileSpeed = 600.f;
	
	TWeakObjectPtr<AVSEnemy> TargetEnemy;
	
	/** 穿透：记录本颗投射物已伤害过的敌人，防止同一次攻击重复扣血（Straight/Homing 共用） */
	TSet<TWeakObjectPtr<AVSEnemy>> HitEnemies;
	
	// NOTE: 本颗投射物可额外穿透的敌人数；<0 表示无限穿透。命中数 > PierceCount 时回池
	int32 PierceCount = 0;
	
	bool bStopOnHit = false;
	
	EVSProjectileMovementMode MovementMode = EVSProjectileMovementMode::Straight;
	
	FVector FlightDirection = FVector::ForwardVector;
};
