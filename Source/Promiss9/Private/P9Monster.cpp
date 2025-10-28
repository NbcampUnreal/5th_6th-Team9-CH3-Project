#include "P9Monster.h"
#include "TimerManager.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

AP9Monster::AP9Monster()
{
    PrimaryActorTick.bCanEverTick = true;

    MonsterMesh = GetMesh();

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

    AttackRangeRadius = 40.f;
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

    PlayHitFlashEffect();



    if (HP <= 0.0f)
    {
        HP = 0.0f;
        Destroy(); 
    }
}

void AP9Monster::PlayHitFlashEffect()
{
    if (MonsterMesh && HitFlashMaterial)
    {
        UMaterialInstanceDynamic* DynMat = MonsterMesh->CreateAndSetMaterialInstanceDynamic(0);
        if (DynMat)
        {
            DynMat->SetScalarParameterValue("HitFlash", 1.0f);

            // 일정 시간 후 원래대로 복구
            FTimerHandle FlashTimer;
            GetWorld()->GetTimerManager().SetTimer(FlashTimer, [DynMat]()
                {
                    DynMat->SetScalarParameterValue("HitFlash", 0.0f);
                }, 0.1f, false);
        }
    }
}

void AP9Monster::ApplyKnockback()
{
    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!Player) return;

    //넉백 방향
    FVector KnockbackDir = (GetActorLocation() - Player->GetActorLocation()).GetSafeNormal();

    //넉백 세기
    float KnockbackStrength = 600.0f; // 상황에 맞게 조절 가능 (300~1000 정도 권장)

    //수직 방향(살짝 위로 밀리는 효과)
    FVector KnockbackVelocity = (KnockbackDir + FVector(0, 0, 0.3f)) * KnockbackStrength;

    //캐릭터 이동 중 넉백 효과 적용
    LaunchCharacter(KnockbackVelocity, true, true);

    //넉백 중 이동 잠깐 제한 (선택사항)
    GetCharacterMovement()->StopMovementImmediately();
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