// LAvid

#pragma once

#include "CoreMinimal.h"
#include "Character/VSCharacterBase.h"
#include "VSEnemy.generated.h"

class UDA_EnemyDropTable;

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
	FORCEINLINE bool IsDead() const {return bIsDead;}
	
	FORCEINLINE UDA_EnemyDropTable* GetDropTable() const { return DropTable; }
	
	
	//设置“移动”速度
	void SetVisualSpeed(float Speed);
	
	//回池前清理
	void OnRecycled();
	
	//从池中取出使用前，先重置状态
	void ResetForSpawn();
	
	virtual float TakeDamage(float Damage, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float VisualSpeed = 200.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float MoveSpeed = 200.f;

	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float DefaultMaxHealth = 30.f;
	
	// 精英相对普通怪的血量倍率
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float EliteHealthMultiplier = 2.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	float Health = 30.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	float MaxHealth = 30.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float BodyScale = 1.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop")
	TObjectPtr<UDA_EnemyDropTable> DropTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	bool bIsElite = false;

private:
	bool bIsDead = false;
	void RefreshDefaultMaxHealth();
	void RefreshDefaultScale();
	
};
