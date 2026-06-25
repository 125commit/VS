// LAvid

#pragma once

#include "CoreMinimal.h"
#include "Actor/VS_WeaponActor.h"
#include "VSGarlicActor.generated.h"

/**
 * 
 */
UCLASS()
class VS_API AVSGarlicActor : public AVS_WeaponActor
{
	GENERATED_BODY()

public:
	AVSGarlicActor();
	
	virtual void ActivateWeapon(const FVSWeaponInitParams& InitParams, AActor* InOwner, APawn* InInstigator) override;
	virtual void DeactivateWeapon() override;
	
	virtual void InitFromParams(const FVSWeaponInitParams& InitParams) override;
	
	/** 每次 Cooldown 到：刷新数值 + 对圈内敌人结算一次周期伤害 */
	void ApplyPeriodicDamageInCircle(const FVSWeaponInitParams& InitParams);
	
protected:
	virtual void BeginPlay() override;
	
	virtual void SweepInitialOverlaps() override;
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult) override;

	// 被动	
	void ApplyGarlicPassiveEffects(AVSEnemy* Enemy);
	
	
	UPROPERTY(EditDefaultsOnly, Category = "VS|Garlic|Collision")
	float BaseSphereRadius = 150.f;
	
	bool bCloseOverlapDamage = false;
	
	// 仅大蒜：命中时削减敌人击退抗性的量(0~1) 与 持续时间(秒)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VS|Garlic|Knockback")
	float KnockbackResistanceReduction = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VS|Garlic|Knockback")
	float ResistanceReductionDuration = 0.7f;
};

