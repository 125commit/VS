// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VS_WeaponActor.generated.h"

class USphereComponent;
class UNiagaraComponent;

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
	
protected:
	
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void HandlingDamage();
	void SweepInitialOverlaps();
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USphereComponent> SphereCollision;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraComponent> WeaponEffect;
	
	
	float WeaponDamage = 0.f;

};
