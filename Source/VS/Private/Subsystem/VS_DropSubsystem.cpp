// LAvid

#include "Subsystem/VS_DropSubsystem.h"
#include "Pool/VSObjectPool.h"
#include "Actor/VS_DropItemActor.h"
#include "Data/Subsystem/VSDropData.h"
#include "Player/VS_PlayerState.h"
#include "Player/VS_PlayerController.h"
#include "AbilitySystem/VS_AttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DataTable.h"


void UVS_DropSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	DropPool = NewObject<UVSObjectPool>(this);

	//初始化设置DropSetting
	DropSettings = Cast<UVS_DropItemDataAsset>(StaticLoadObject(
		UVS_DropItemDataAsset::StaticClass(),
		nullptr,
		TEXT("/Game/Data/DA/DA_DropItems.DA_DropItems")
	));
	
	if (!DropSettings)
	{
		UE_LOG(LogTemp, Error, TEXT("VSDropManager: Failed to load DA_DropItems at /Game/Data/DA_DropItems"));
	}
	
	if (DropSettings)
	{
		MagnetRadius = DropSettings->DefaultMagnetRadius;
		MagnetMoveSpeed = DropSettings->MagnetMoveSpeed;
		PickupRadius = DropSettings->PickupRadius;
	}

	MagnetRadiusSq = MagnetRadius * MagnetRadius;

	PreWarmPools();
}

void UVS_DropSubsystem::Deinitialize()
{
	TArray<TObjectPtr<AVS_DropItemActor>> DropsToReturn = MoveTemp(ActiveDrops);
	for (AVS_DropItemActor* Drop : DropsToReturn)
	{
		if (!DropPool || !Drop) continue;
		Drop->DeActivateDrop();
		DropPool->ReturnActorToPool(Drop);
	}
	ActiveDrops.Empty();

	if (DropPool)
	{
		DropPool->ClearPool();
	}

	Super::Deinitialize();
}

void UVS_DropSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (ActiveDrops.Num() == 0) return;

	TryPickUp(DeltaTime);
}

TStatId UVS_DropSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UVS_DropSubsystem, STATGROUP_Tickables);
}

void UVS_DropSubsystem::SpawnDrop(const FVector& Location, const UDataTable* DropTable)
{
	UWorld* World = GetWorld();
	if (!World || !DropPool || !DropSettings || !DropTable) return;

	TArray<FVSDropEntry*> AllRows;
	DropTable->GetAllRows<FVSDropEntry>(TEXT("SpawnDrop"), AllRows);

	if (AllRows.Num() == 0) return;

	FVSDropEntry SelectedEntry;
	bool bFound = false;

	for (const FVSDropEntry* EntryRow : AllRows)
	{
		if (EntryRow->DropType == EVSDropType::None) continue;

		if (EntryRow->bGuaranteed || FMath::FRand() <= EntryRow->DropChance)
		{
			SelectedEntry = *EntryRow;
			bFound = true;
			break;
		}
	}

	if (!bFound) return;

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("Drop Item"));
	
	const FVSDropTypeDefinition DropTypeDefinition = DropSettings->FindDefinition(SelectedEntry.DropType);
	const TSubclassOf<AVS_DropItemActor> DropClass = DropTypeDefinition.DropActorClass;
	if (DropClass == nullptr) return;

	//Spawn
	AActor* Actor = DropPool->GetActorFromPool(World, DropClass, Location, FRotator::ZeroRotator);
	AVS_DropItemActor* DropItem = Cast<AVS_DropItemActor>(Actor);
	check(DropItem);

	DropItem->ActivateDrop(SelectedEntry.DropType, SelectedEntry.DropValue);
	ActiveDrops.Add(DropItem);
}

void UVS_DropSubsystem::TryPickUp(float DeltaTime)
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn) return;

	AVS_PlayerState* PlayerState = PlayerPawn->GetPlayerState<AVS_PlayerState>();
	if (!PlayerState) return;

	
	const FVector PlayerLoc = PlayerPawn->GetActorLocation();

	const float CurrentMagnetRadiusSq = GetPlayerMagnetRadiusSq(PlayerPawn);
	const float PickupSq = GetPlayerPickupRadiusSq();

	for (int32 i = ActiveDrops.Num() - 1; i >= 0; --i)
	{
		AVS_DropItemActor* Drop = ActiveDrops[i];
		if (!IsValid(Drop) || !Drop->IsActive())
		{
			ActiveDrops.RemoveAtSwap(i);
			continue;
		}
		const FVector DropLoc = Drop->GetActorLocation();
		const float DistSq = FVector::DistSquared(PlayerLoc, DropLoc);

		if (DistSq <= PickupSq)
		{
			HandleCollectingDrop(Drop, PlayerState);
			continue;
		}

		if (DistSq <= CurrentMagnetRadiusSq)
		{
			FVector Dir = PlayerLoc - DropLoc;
			Dir.Z = 0.f;
			if (!Dir.IsNearlyZero())
			{
				Dir.Normalize();
				Drop->SetActorLocation(DropLoc + Dir * MagnetMoveSpeed * DeltaTime, false);
			}
		}
	}
}

void UVS_DropSubsystem::HandleCollectingDrop(AVS_DropItemActor* DropItem, AVS_PlayerState* PlayerState)
{
	if (!DropItem || !DropPool || !PlayerState) return;
	switch (DropItem->GetDropType())
	{
	case EVSDropType::XPSmall:
	case EVSDropType::XPLarge:
		PlayerState->AddXP(DropItem->GetDropValue());
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Get XP: [%f]"), DropItem->GetDropValue()));
		break;

	case EVSDropType::Gold:
		break;

	case EVSDropType::Turkey:
		break;

	case EVSDropType::Box:
		if (AVS_PlayerController* PC = Cast<AVS_PlayerController>(PlayerState->GetPlayerController()))
		{
			PC->Server_ProcessChestPickup(FMath::RoundToInt(DropItem->GetDropValue()));
		}
		break;

	default:
		break;
	}

	DropItem->DeActivateDrop();
	ActiveDrops.RemoveSingleSwap(DropItem);
	DropPool->ReturnActorToPool(DropItem);
}

float UVS_DropSubsystem::GetPlayerMagnetRadiusSq(const APawn* PlayerPawn)
{
	if (!PlayerPawn) return MagnetRadiusSq;

	if (AVS_PlayerState* PS = Cast<AVS_PlayerState>(PlayerPawn->GetPlayerState()))
	{
		if (UVS_AttributeSet* AS = PS->GetAttributeSet())
		{
			const float CurrentMagnetRadius = AS->GetMagnetRadius();
			return CurrentMagnetRadius * CurrentMagnetRadius;
		}
	}
	return MagnetRadiusSq;
}

float UVS_DropSubsystem::GetPlayerPickupRadiusSq()
{
	return PickupRadius * PickupRadius;
}

void UVS_DropSubsystem::PreWarmPools()
{
	if (!DropPool || !DropSettings) return;

	UWorld* World = GetWorld();
	if (!World) return;

	for (const TPair<EVSDropType, FVSDropTypeDefinition>& Pair : DropSettings->DropTypeToTypeDef)
	{
		const FVSDropTypeDefinition& Def = Pair.Value;
		if (!Def.DropActorClass || Def.PreSpawnCount <= 0) continue;

		for (int32 i = 0; i < Def.PreSpawnCount; ++i)
		{
			AActor* Actor = DropPool->GetActorFromPool(World, Def.DropActorClass, FVector::ZeroVector, FRotator::ZeroRotator);

			if (AVS_DropItemActor* DropItem = Cast<AVS_DropItemActor>(Actor))
			{
				DropItem->DeActivateDrop();
				DropPool->ReturnActorToPool(DropItem);
			}
		}
	}
}
