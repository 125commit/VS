// LAvid


#include "Actor/VSBibleActor.h"

#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "Character/VSEnemy.h"
#include "Kismet/GameplayStatics.h"

AVSBibleActor::AVSBibleActor()
{
	// 公转依赖每帧更新位置，必须开启 Tick
	PrimaryActorTick.bCanEverTick = true;

	// NOTE: 半径不在构造函数里设置——构造时拿不到蓝图覆盖的 DefaultSphereRadius，
	//       会永远用 C++ 默认值。改到 BeginPlay 应用（见 BeginPlay）
}

void AVSBibleActor::InitFromParams(const FVSWeaponInitParams& InitParams)
{
	// NOTE: 不调用 Super::InitFromParams，避免基类用 SetActorScale3D(Area) 把命中球按 Area 二次放大
	WeaponDamage = InitParams.Damage;

	// 借用 FacingRotation.Yaw 传初始相位角（圣经无朝向需求，复用该字段免新增结构字段）
	CurrentAngleDeg = InitParams.FacingRotation.Yaw;

	// 公转半径随 Area 缩放；Area 兜底防 0
	Radius = BaseRadius * FMath::Max(0.01f, InitParams.Area);

	// 转速 = 基准转速 × 该级速度倍率（表里 SpeedMultiplier 填 1.3 即 +30% 转速）
	AngularSpeedDegPerSec = BaseAngularSpeedDegPerSec * FMath::Max(0.f, InitParams.SpeedMultiplier);
}

void AVSBibleActor::ActivateWeapon(const FVSWeaponInitParams& InitParams, AActor* InOwner, APawn* InInstigator)
{
	// 仅服务器驱动逻辑（与项目内其它武器一致）
	if (!HasAuthority()) return;

	InitFromParams(InitParams);

	SetOwner(InOwner);
	SetInstigator(InInstigator);
	SetActorHiddenInGame(false);

	// STEP: 出池瞬间直接摆到环绕点，避免"先停在玩家身上、下一帧再弹出去"的跳变
	if (AActor* OwnerActor = GetOwner())
	{
		const float Rad = FMath::DegreesToRadians(CurrentAngleDeg);
		const FVector Center = OwnerActor->GetActorLocation();
		SetActorLocation(Center + FVector(Radius * FMath::Cos(Rad), Radius * FMath::Sin(Rad), 0.f));
	}

	// QueryOnly + 生成重叠事件：只检测、不产生物理阻挡
	if (SphereCollision)
	{
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SphereCollision->SetGenerateOverlapEvents(true);
	}
	SetActorEnableCollision(true);

	if (WeaponEffect)
	{
		WeaponEffect->Activate(true);
	}

	// STEP: 在场时长 = Duration，到点交给基类 OnLifetimeEnd 回池（无 Duration 时给 3s 兜底）
	const float Duration = InitParams.Duration > 0.f ? InitParams.Duration : 3.f;
	GetWorldTimerManager().ClearTimer(LifetimeTimerHandle);
	GetWorldTimerManager().SetTimer(
		LifetimeTimerHandle,
		this,
		&AVSBibleActor::OnLifetimeEnd,
		Duration,
		false);
}

void AVSBibleActor::DeactivateWeapon()
{
	// 回池前清理运行时状态，保证下次出池干净
	CurrentAngleDeg = 0.f;
	Radius = 0.f;
	AngularSpeedDegPerSec = 0.f;

	// 交给基类完成隐藏、关碰撞、清定时器等通用回收
	Super::DeactivateWeapon();
}

void AVSBibleActor::BeginPlay()
{
	Super::BeginPlay();

	// WARN: 池化 Actor 只在首次创建时进 BeginPlay，重叠回调在此绑定一次即可（出/回池不重复绑定）
	if (HasAuthority() && SphereCollision)
	{
		SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AVSBibleActor::OnOverlapBegin);
	}
}

void AVSBibleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority()) return;

	// NOTE: 以玩家"当前"位置为圆心（不附着玩家），从而不受玩家自身朝向旋转的耦合
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor) return;

	CurrentAngleDeg += AngularSpeedDegPerSec * DeltaTime;

	const float Rad = FMath::DegreesToRadians(CurrentAngleDeg);
	const FVector Center = OwnerActor->GetActorLocation();
	const FVector Offset(Radius * FMath::Cos(Rad), Radius * FMath::Sin(Rad), 0.f);

	// WARN: bSweep=true 不可省——提速后单帧位移变大，靠扫掠检测才能避免一帧跳过敌人导致漏触发重叠
	SetActorLocation(Center + Offset, /*bSweep=*/true);
}

void AVSBibleActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (SphereCollision)
	{
		SphereCollision->SetSphereRadius(DefaultSphereRadius);
	}
}

void AVSBibleActor::SweepInitialOverlaps()
{
	// 故意留空，理由见头文件注释
}

void AVSBibleActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 过滤无效目标与玩家自身
	if (!HasAuthority() || !OtherActor || OtherActor == this || OtherActor == GetInstigator()) return;

	AVSEnemy* Enemy = Cast<AVSEnemy>(OtherActor);
	if (!Enemy || Enemy->IsDead() || WeaponDamage <= 0.f) return;

	// NOTE: 每次重叠开始即结算一次伤害——书每转一圈扫过敌人一次=命中一次，转速越快命中越频繁
	UGameplayStatics::ApplyDamage(
		Enemy,
		WeaponDamage,
		GetInstigatorController(),
		this,
		UDamageType::StaticClass());
}
