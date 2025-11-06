#include "P9Monster.h"
#include "P9PlayerState.h"
#include "P9HealingItem.h"
#include "TimerManager.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h" //추가
#include "Blueprint/UserWidget.h" // 추가
#include "Components/TextBlock.h" // 추가
#include "Components/CapsuleComponent.h" 
#include "UObject/ConstructorHelpers.h" //추가

AP9Monster::AP9Monster()
{
    PrimaryActorTick.bCanEverTick = true;

    // 위젯 블루프린트 클래스 자동 지정
    static ConstructorHelpers::FClassFinder<UUserWidget> DamageWidgetBP(
        TEXT("/Game/Blueprints/UI/WBP_DamageText")
    );

    if (DamageWidgetBP.Succeeded())
    {
        DamageTextWidgetClass = DamageWidgetBP.Class;
    }

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
    if (IsActorBeingDestroyed())
    {
        UE_LOG(LogTemp, Warning, TEXT("디졸브 중단: 이미 파괴 중인 액터"));
        return;
    }

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
			SetLifeSpan(0.2f); // 짧은 시간 후에 액터 제거
        }
    }
}

void AP9Monster::TakeDamageFromPlayer(float DamageAmount, bool Critical)
{
    UE_LOG(LogTemp, Warning, TEXT("데미지 함수 실행!"));


    HP -= DamageAmount;

    PlayHitFlashEffect();

    if (DamageAmount > 0.f)
    {
        ShowDamageWidget(DamageAmount, Critical);
        UE_LOG(LogTemp, Warning, TEXT("데미지 : %.0f"), DamageAmount);
    }

    if (!bIsBossMonster) {
        ApplyKnockback();
    }

    if (HP <= 0.0f)
    {
        HP = 0.0f;
		Die();
    }
}

void AP9Monster::PlayHitFlashEffect()
{
    if (IsActorBeingDestroyed())
    {
        UE_LOG(LogTemp, Warning, TEXT("플래시 중단: 이미 파괴 중인 액터"));
        return;
    }

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
    bIsDead = true;

    if (CurrentState == EMonsterState::Dead)
        return;

    CurrentState = EMonsterState::Dead;

    UE_LOG(LogTemp, Warning, TEXT("Die() 호출됨"));

    // 움직임 정지
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->DisableMovement();
    }

    //보상 증가
    if (APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController()))
    {
        if (AP9PlayerState* PS = PC->GetPlayerState<AP9PlayerState>())
        {
            PS->AddXP(ExpReward);
			PS->AddGold(GoldReward);
			PS->AddKillCount();
			AP9HealingItem::SpawnHealingItem(GetWorld(), this);
        }
    }
   
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
    if (HitFlashMatInstance && !IsActorBeingDestroyed())
    {
        CurrentDissolveValue = 0.0f;
        HitFlashMatInstance->SetScalarParameterValue("DissolveAmount", 0.0f);

        bIsDissolving = true;

        UE_LOG(LogTemp, Warning, TEXT("투명화 시작"));
    }
}

void AP9Monster::ShowDamageWidget(float DamageValue, bool bIsCritical)
{
    if (bIsDead) return;
    if (!DamageTextWidgetClass) return;

    FTimerHandle FaceTimer;
    FTimerHandle RemoveTimer;

    UWidgetComponent* DamageWidgetComp = NewObject<UWidgetComponent>(this);
    if (!DamageWidgetComp) return;

    DamageWidgetComp->RegisterComponent();
    DamageWidgetComp->SetWidgetSpace(EWidgetSpace::World);
    DamageWidgetComp->SetDrawAtDesiredSize(true);
    DamageWidgetComp->SetPivot(FVector2D(0.5f, 0.5f));
    DamageWidgetComp->SetTwoSided(true);
    DamageWidgetComp->SetWidgetClass(DamageTextWidgetClass);
    DamageWidgetComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

    // 위젯 띄우는 위치
    float HalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    float OffsetZ = HalfHeight + 80.f; // 머리 위 약간 띄우기 / 해당 수치로 위치 조절 가능
    FVector RandomOffset(
        FMath::RandRange(-30.f, 30.f),   // 좌우로 살짝 퍼지게
        FMath::RandRange(-30.f, 30.f),   // 앞뒤로 살짝 퍼지게
        FMath::RandRange(-10.f, 20.f)    // 높이도 살짝 랜덤
    );

    DamageWidgetComp->SetRelativeLocation(FVector(0.f, 0.f, OffsetZ) + RandomOffset);

    // 텍스트 설정
    if (UUserWidget* Widget = DamageWidgetComp->GetUserWidgetObject())
    {
        UTextBlock* DamageTextBlock = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("DamageText")));
        if (DamageTextBlock)
        {
            FString DamageString = FString::Printf(TEXT("%.0f"), DamageValue);
            DamageTextBlock->SetText(FText::FromString(DamageString));

            if (bIsCritical)
            {
                DamageTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::Red)); // 빨간색
                DamageTextBlock->SetRenderScale(FVector2D(1.2f, 1.2f)); // 크기 조정
            }
            else
            {
                DamageTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::White));
                DamageTextBlock->SetRenderScale(FVector2D(1.0f, 1.0f));
            }
        }
    }

    TWeakObjectPtr<UWidgetComponent> WeakDamageWidget = DamageWidgetComp;

    // 카메라 바라보기 타이머
    GetWorld()->GetTimerManager().SetTimer(
        FaceTimer,
        [WeakDamageWidget]()
        {
            if (!WeakDamageWidget.IsValid()) return;

            APlayerController* PC = UGameplayStatics::GetPlayerController(WeakDamageWidget->GetWorld(), 0);
            if (PC && PC->PlayerCameraManager)
            {
                FVector CamLoc = PC->PlayerCameraManager->GetCameraLocation();
                FVector ToCam = CamLoc - WeakDamageWidget->GetComponentLocation();
                WeakDamageWidget->SetWorldRotation(ToCam.Rotation());
            }
        },
        0.02f, true
    );

    // 기존 제거 타이머 안에 FaceTimer 해제도 추가
    GetWorld()->GetTimerManager().SetTimer(RemoveTimer, [this, WeakDamageWidget, &FaceTimer]()
        {
            if (!IsValid(this)) return;

            if (WeakDamageWidget.IsValid())
            {
                WeakDamageWidget->DestroyComponent();
            }

            if (GetWorld())
            {
                GetWorld()->GetTimerManager().ClearTimer(FaceTimer);
            }
        },
        0.8f, false);
}

