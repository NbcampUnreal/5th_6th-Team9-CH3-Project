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

        if (CurrentDissolveValue >= 0.5f)
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

    //(주의) 여기서는 isDead를 블루프린트에서 직접 세팅하므로 코드에서 할 필요 없음

    //1.33초 뒤에 디졸브 시작
    FTimerHandle DeathTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(DeathTimerHandle, [this]()
        {
            UE_LOG(LogTemp, Warning, TEXT("사망 애니메이션 완료, 디졸브 시작"));
            StartDissolveEffect();
        },
        1.33f, false
    );
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

