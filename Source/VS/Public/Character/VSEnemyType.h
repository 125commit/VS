#pragma once

#include "CoreMinimal.h"
#include "VSEnemyType.generated.h"

UENUM(BlueprintType)
enum class EVSEnemyType : uint8
{
	None UMETA(DisplayName = "None"),
	Goblin UMETA(DisplayName = "Goblin"),
	Slime UMETA(DisplayName = "Slime"),
	Bat UMETA(DisplayName = "Bat"),
	Boss UMETA(DisplayName = "Boss")
};
