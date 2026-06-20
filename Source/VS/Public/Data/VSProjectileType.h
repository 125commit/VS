#pragma once

#include "CoreMinimal.h"
#include "VSProjectileType.generated.h"

/** GA 选目标方式 */
UENUM(BlueprintType)
enum class EVSProjectileTargetMode : uint8
{
	/** 强能魔杖：最近敌人 */
	NearestEnemy UMETA(DisplayName = "Nearest Enemy"),
	/** 火焰魔杖：随机敌人 */
	RandomEnemy  UMETA(DisplayName = "Random Enemy"),
};

/** Actor 飞行方式 */
UENUM(BlueprintType)
enum class EVSProjectileMovementMode : uint8
{
	/** 火焰魔杖：spawn 时确定方向，之后直线 */
	Straight UMETA(DisplayName = "Straight"),
	/** 强能魔杖：持续追踪 TargetEnemy */
	Homing   UMETA(DisplayName = "Homing"),
};