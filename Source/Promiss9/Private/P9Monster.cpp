#include "P9Monster.h"
#include "TimerManager.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

AP9Monster::AP9Monster()
{
    PrimaryActorTick.bCanEverTick = true;

    // 기본값
    HP = 100.0f;
    AttackPower = 10.0f;
    ExpReward = 10;
    GoldReward = 5;
    DamageInterval = 1.0f;
    bIsPlayerInRange = false;
    TargetPlayer = nullptr;

    AttackRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRangeSphere"));
    AttackRangeSphere->SetupAttachment(RootComponent);

    AttackRangeRadius = 200.f; // 에디터에서 조정 가능 (기본값)
    AttackRangeSphere->InitSphereRadius(AttackRangeRadius);

    AttackRangeSphere->SetCollisionProfileName(TEXT("Trigger"));
    AttackRangeSphere->SetGenerateOverlapEvents(true);

}

void AP9Monster::BeginPlay()
{
    Super::BeginPlay();
}

void AP9Monster::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void AP9Monster::TakeDamageFromPlayer(float DamageAmount)
{
    HP -= DamageAmount;

    if (HP <= 0.0f)
    {
        HP = 0.0f;
        Destroy(); 
    }
}

void AP9Monster::StartDamagePlayer(AActor* PlayerActor)
{
    if (!PlayerActor) return;

    TargetPlayer = PlayerActor;
    bIsPlayerInRange = true;

    // 일정 간격으로 데미지 적용
    GetWorldTimerManager().SetTimer(
        DamageTimerHandle,
        this,
        &AP9Monster::DealDamageToPlayer,
        DamageInterval,
        true
    );
}

void AP9Monster::StopDamagePlayer()
{
    bIsPlayerInRange = false;
    TargetPlayer = nullptr;
    GetWorldTimerManager().ClearTimer(DamageTimerHandle);
}

void AP9Monster::DealDamageToPlayer()
{
    if (!bIsPlayerInRange || !TargetPlayer) return;

    // 실제 데미지 전달은 블루프린트에서 구현 가능 (예: 플레이어의 BP 함수 호출)
    // 여기선 예시로 출력만
    UE_LOG(LogTemp, Warning, TEXT("%s attacks %s for %.1f damage!"),
        *GetName(), *TargetPlayer->GetName(), AttackPower);
}

void AP9Monster::OnAttackRangeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this)
    {
        StartDamagePlayer(OtherActor);
    }
}

void AP9Monster::OnAttackRangeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor == TargetPlayer)
    {
        StopDamagePlayer();
    }
}