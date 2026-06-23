// LAvid

#pragma once

#include "CoreMinimal.h"
#include "Actor/VS_DropItemActor.h"
#include "Data/Subsystem/VSDropData.h"
#include "Subsystems/WorldSubsystem.h"
#include "VS_DropSubsystem.generated.h"

class AVS_PlayerState;
class UDataTable;
class UVSObjectPool;

UCLASS()
class VS_API UVS_DropSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickableInEditor() const override { return false; }

	void SpawnDrop(const FVector& Location, const UDataTable* DropTable);
	void TryPickUp(float DeltaTime);

protected:
	UPROPERTY()
	TObjectPtr<UVS_DropItemDataAsset> DropSettings;

	UPROPERTY()
	TObjectPtr<UVSObjectPool> DropPool = nullptr;

	UPROPERTY()
	TArray<TObjectPtr<AVS_DropItemActor>> ActiveDrops;

private:
	void PreWarmPools();

	float GetPlayerMagnetRadiusSq(const APawn* PlayerPawn);
	float GetPlayerPickupRadiusSq();
	void HandleCollectingDrop(AVS_DropItemActor* DropItem, AVS_PlayerState* PlayerState);

	float MagnetRadius = 100.f;
	float MagnetRadiusSq = 10000.f;
	float MagnetMoveSpeed = 500.f;

	float PickupRadius = 50.f;
};
