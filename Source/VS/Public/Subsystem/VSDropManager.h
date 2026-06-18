// LAvid

#pragma once

#include "CoreMinimal.h"
#include "Actor/VSDropItem.h"
#include "Data/Subsystem/DA_DropItems.h"
#include "Subsystems/worldSubsystem.h"
#include "VSDropManager.generated.h"

class AVS_PlayerState;
class UDA_EnemyDropTable;
class UVSObjectPool;
struct FDropTypeDefinition;

UCLASS()
class VS_API UVSDropManager : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickableInEditor() const override { return false; }

	void SpawnDrop(const FVector& Location, const UDA_EnemyDropTable* DropTable);
	void TryPickUp(float DeltaTime);

protected:
	UPROPERTY()
	TObjectPtr<UDA_DropItems> DropSettings;

	UPROPERTY()
	TObjectPtr<UVSObjectPool> DropPool = nullptr;

	UPROPERTY()
	TArray<TObjectPtr<AVSDropItem>> ActiveDrops;

private:
	void PreWarmPools();

	float GetPlayerMagnetRadiusSq(const APawn* PlayerPawn);
	float GetPlayerPickupRadiusSq();
	void HandleColletingDrop(AVSDropItem* DropItem, AVS_PlayerState* PlayerState);

	float MagnetRadius = 100.f;
	float MagnetRadiusSq = 10000.f;
	float MagnetMoveSpeed = 500.f;

	float PickupRadius = 50.f;
};
