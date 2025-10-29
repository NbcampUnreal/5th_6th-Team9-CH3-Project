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

    if (MonsterMesh)
    {
        UMaterialInterface* BaseMat = MonsterMesh->GetMaterial(0);
        if (BaseMat)
        {
            HitFlashMatInstance = MonsterMesh->CreateAndSetMaterialInstanceDynamicFromMaterial(0, BaseMat);
        }
    }
}

void AP9Monster::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Dissolve 진행
    if (bIsDissolving && HitFlashMatInstance)
    {
        CurrentDissolveValue += DeltaTime / DissolveDuration;
        HitFlashMatInstance->SetScalarParameterValue("DissolveAmount", CurrentDissolveValue);

        if (CurrentDissolveValue >= 1.0f)
        {
            UE_LOG(LogTemp, Warning, TEXT("투명화 끝남"));
            bIsDissolving = false;
            Destroy();
        }
    }
}

void AP9Monster::TakeDamageFromPlayer(float DamageAmount)
{
    UE_LOG(LogTemp, Warning, TEXT("데미지 함수 실행!"));


    HP -= DamageAmount;

    PlayHitFlashEffect();

    ApplyKnockback();

    if (HP <= 0.0f)
    {
        HP = 0.0f;
		Die();
    }
}

void AP9Monster::PlayHitFlashEffect()
{
    if (HitFlashMatInstance)
    {
        HitFlashMatInstance->SetScalarParameterValue("HitFlash", 1.0f);

        FTimerHandle FlashTimer;
        GetWorld()->GetTimerManager().SetTimer(FlashTimer, [this]()
            {
                if (HitFlashMatInstance)
                    HitFlashMatInstance->SetScalarParameterValue("HitFlash", 0.0f);
            }, 0.1f, false);
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

void AP9Monster::Die()
{
    if (CurrentState == EMonsterState::Dead)
        return;

    CurrentState = EMonsterState::Dead;

    UE_LOG(LogTemp, Warning, TEXT("Die() 호출됨"));

    // 움직임 정지
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->DisableMovement();
    }

    if (DeathAnimMontage && MonsterMesh && MonsterMesh->GetAnimInstance())
    {
        UE_LOG(LogTemp, Warning, TEXT("사망 애니메이션 호출됨"));

        UAnimInstance* AnimInstance = MonsterMesh->GetAnimInstance();

        // 사망 애니메이션 재생
        AnimInstance->Montage_Play(DeathAnimMontage);

        // Montage 종료 시점 처리용 델리게이트
        FOnMontageEnded MontageEndedDelegate;
        MontageEndedDelegate.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
            {
                UE_LOG(LogTemp, Warning, TEXT("사망 몽타주 종료됨, 포즈 유지 및 디졸브 시작"));

                if (MonsterMesh)
                {
                    // 현재 포즈 그대로 유지
                    MonsterMesh->bPauseAnims = true;
                }

                // 디졸브 이펙트 시작
                StartDissolveEffect();
            });

        // 델리게이트 등록
        AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, DeathAnimMontage);
    }
    else
    {
        // 사망 몽타주가 없으면 바로 디졸브
        StartDissolveEffect();
    }
}

void AP9Monster::StartDissolveEffect()
{
    if (HitFlashMatInstance)
    {
        CurrentDissolveValue = 0.0f;
        HitFlashMatInstance->SetScalarParameterValue("DissolveAmount", 0.0f);

        bIsDissolving = true;

        UE_LOG(LogTemp, Warning, TEXT("투명화 시작"));
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