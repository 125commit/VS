// LAvid

#pragma once

#include "CoreMinimal.h"
#include "Character/VSCharacterBase.h"
#include "VSEnemy.generated.h"

/**
 * 
 */
UCLASS()
class VS_API AVSEnemy : public AVSCharacterBase
{
	GENERATED_BODY()
	
public:
	AVSEnemy();
	
	void SetIsElite(bool bIsInElite);
	FORCEINLINE bool GetIsElite() {return bIsElite;}
	
	void SetVisualSpeed(float Speed);
	
	void OnRecycled();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float VisualSpeed = 200.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float MoveSpeed = 200.f;

	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	bool bIsElite = false;

	
};
