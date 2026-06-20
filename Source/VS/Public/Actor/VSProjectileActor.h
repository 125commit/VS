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
	
	/** FireWand 穿透：记录本颗投射物已伤害过的敌人，防止同一次攻击重复扣血 */
	TSet<TWeakObjectPtr<AVSEnemy>> HitEnemies;
	
	bool bStopOnHit = false;
	
	EVSProjectileMovementMode MovementMode = EVSProjectileMovementMode::Straight;
	
	FVector FlightDirection = FVector::ForwardVector;
};
