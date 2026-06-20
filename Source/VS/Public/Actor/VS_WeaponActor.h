// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	
	UPROPERTY(BlueprintReadOnly, Category = "VS|Weapon")
	TWeakObjectPtr<AVSEnemy> TargetEnemy;
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
	
	virtual void BeginPlay() override;
	
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

};
