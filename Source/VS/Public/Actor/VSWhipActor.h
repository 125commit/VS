// LAvid

#pragma once

#include "CoreMinimal.h"
#include "Actor/VS_WeaponActor.h"
#include "VSWhipActor.generated.h"

class UBoxComponent;
/**
 * 
 */
UCLASS()
class VS_API AVSWhipActor : public AVS_WeaponActor
{
	GENERATED_BODY()
	
public:
	AVSWhipActor();
	
	virtual void InitFromParams(const FVSWeaponInitParams& InitParams) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
	void SetWhipCollision();
	// Whip专用的 Box碰撞体 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VS|Whip")
	TObjectPtr<UBoxComponent> WhipCollision;
	
	// Box 半尺寸（Area=1 时的设计基准，单位 cm）
	UPROPERTY(EditDefaultsOnly, Category = "VS|Whip|Collision")
	FVector BaseBoxExtent = FVector(150.f, 200.f, 60.f);
	
	// Box 中心相对 Actor 原点的偏移（让鞭击从角色前方打出）
	UPROPERTY(EditDefaultsOnly, Category = "VS|Whip|Collision")
	FVector BaseBoxOffset = FVector(150.f, 0.f, 60.f);


};
