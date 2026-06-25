#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Character/VSEnemyType.h"
#include "DA_EnemyDictionary.generated.h"

class AVSEnemy;
class UGameplayEffect;
class UDataTable;

/** DataTable 行：策划用 Excel/CSV 配置的纯数值（RowName 用 EVSEnemyType 名字：Goblin/Slime/Bat/Boss/None） */
USTRUCT(BlueprintType)
struct FVSEnemyStatRow : public FTableRowBase
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    float MaxHealth = 30.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    float MoveSpeed = 200.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    float BodyScale = 1.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
    float AttackDamage = 5.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
    float AttackInterval = 1.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Knockback",
        meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float KnockbackResistance = 0.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Elite")
    float EliteHealthMultiplier = 2.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Elite")
    float EliteScaleMultiplier = 1.3f;
    
    // 注意：DropTable 也是资源引用，放进 DataTable 时 CSV 里要填资源路径
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drop")
    TObjectPtr<UDataTable> DropTable;
};

/** 一个 Type 对应的全部数据入口：类引用 + 指向数值行的句柄 */
USTRUCT(BlueprintType)
struct FVSEnemyInfo
{
    GENERATED_BODY()
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Class")
    TSubclassOf<AVSEnemy> EnemyClass;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
    TSubclassOf<UGameplayEffect> DamageEffectClass;
    
    // 编辑器里：先选 DataTable，再从下拉框选这一行（RowType 过滤只显示 FVSEnemyStatRow 的表）
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats",
        meta = (RowType = "/Script/VS.VSEnemyStatRow"))
    FDataTableRowHandle StatRow;
};

/** 运行时合并出来的完整定义（数值来自 DataTable，类来自 DataAsset） */
USTRUCT(BlueprintType)
struct FVSEnemyDefinition
{
    GENERATED_BODY()
    
    TSubclassOf<AVSEnemy> EnemyClass;
    
    TSubclassOf<UGameplayEffect> DamageEffectClass;
    
    float MaxHealth = 30.f;
    float MoveSpeed = 200.f;
    float BodyScale = 1.f;
    float AttackDamage = 5.f;
    float AttackInterval = 1.f;
    float KnockbackResistance = 0.f;
    float EliteHealthMultiplier = 2.f;
    float EliteScaleMultiplier = 1.3f;
   
    UPROPERTY()
    TObjectPtr<UDataTable> DropTable;
};


/**
 * 怪物字典：EnemyType -> 完整定义（唯一数据源）
 */
UCLASS()
class VS_API UDA_EnemyDictionary : public UDataAsset
{
    GENERATED_BODY()

public:
    // 唯一数据源：Type -> 该 Type 的全部数据入口
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnemyDictionary")
    TMap<EVSEnemyType, FVSEnemyInfo> Enemies;

    bool GetEnemyDefinition(EVSEnemyType Type, FVSEnemyDefinition& OutDef) const;

    // 兼容旧调用：仅取类
    UFUNCTION(BlueprintCallable, Category = "EnemyDictionary")
    TSubclassOf<AVSEnemy> GetEnemyClass(EVSEnemyType Type) const;
};