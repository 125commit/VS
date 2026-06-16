// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VS_WeaponActor.generated.h"

class USphereComponent;
class UNiagaraComponent;

/**
 * 实体武器打手基类
 * 负责展现特效、处理碰撞检测并施加伤害
 */
UCLASS()
class VS_API AVS_WeaponActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AVS_WeaponActor();

	// 提供给 Ability 调用的初始化接口
	// @param InArea 范围缩放倍率
	// @param InDuration 存活时间
	// @param InDamage 最终计算好的伤害数值
	void InitWeapon(float InArea, float InDuration, float InDamage);

protected:
	virtual void BeginPlay() override;

	// 碰撞体
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VS|Weapon")
	TObjectPtr<USphereComponent> CollisionSphere;

	// 特效体
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VS|Weapon")
	TObjectPtr<UNiagaraComponent> WeaponEffect;

	// 重叠检测
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	// 保存 Ability 传过来的最终伤害数值
	float WeaponDamage = 0.f;
};
