// LAvid

#pragma once

#include "CoreMinimal.h"
#include "Actor/VS_WeaponActor.h"
#include "VSBibleActor.generated.h"

class AVSEnemy;

/**
 * 职责：国王圣经（魔法书）的打手 Actor——每帧绕玩家公转，扫过敌人即造成一次伤害
 * 原因：圣经是"常驻同位置 + 按级数量固定"的武器，运动需逐帧计算公转位置；
 *       伤害走"重叠即触发"，转速越快扫过越频繁，从而提速时 DPS 自然提升
 */
UCLASS()
class VS_API AVSBibleActor : public AVS_WeaponActor
{
	GENERATED_BODY()

public:
	AVSBibleActor();

protected:
	virtual void InitFromParams(const FVSWeaponInitParams& InitParams) override;
	virtual void ActivateWeapon(const FVSWeaponInitParams& InitParams, AActor* InOwner, APawn* InInstigator) override;
	virtual void DeactivateWeapon() override;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	// 故意置空：圣经靠公转自然扫过敌人，无需出池瞬间补扫（否则会与"扫过即伤害"重复计伤）
	virtual void SweepInitialOverlaps() override;

	virtual void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;

	// 命中判定球半径。WARN: 必须明显小于 BaseRadius，保证每圈能脱离敌人、从而能再次触发重叠
	UPROPERTY(EditDefaultsOnly, Category = "VS|Bible")
	float DefaultSphereRadius = 40.f;

	// Area=1 时的公转半径；实际半径 = BaseRadius * Area
	UPROPERTY(EditDefaultsOnly, Category = "VS|Bible")
	float BaseRadius = 150.f;

	// SpeedMultiplier=1.0 时的基准转速（度/秒），决定整体旋转手感
	UPROPERTY(EditDefaultsOnly, Category = "VS|Bible")
	float BaseAngularSpeedDegPerSec = 120.f;

	// ---- 运行时状态（出池时按 InitParams 重算）----

	// 当前相位角（度），每帧累加
	float CurrentAngleDeg = 0.f;

	// 实际公转半径
	float Radius = 0.f;

	// 实际角速度（度/秒）
	float AngularSpeedDegPerSec = 0.f;
};
