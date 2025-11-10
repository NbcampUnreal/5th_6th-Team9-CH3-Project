// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageTextWidgetComponent.h"
#include "Kismet/GameplayStatics.h"




void UDamageTextWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

    // 카메라 바라보기 타이머
    GetWorld()->GetTimerManager().SetTimer(
        FaceTimer,
        [this]()
        {
            if (!IsValid(this)) return;

            APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
            if (PC && PC->PlayerCameraManager)
            {
                FVector CamLoc = PC->PlayerCameraManager->GetCameraLocation();
                FVector ToCam = CamLoc - GetComponentLocation();
                SetWorldRotation(ToCam.Rotation());
            }
        },
        0.02f, true
    );

    // 기존 제거 타이머 안에 FaceTimer 해제도 추가
    GetWorld()->GetTimerManager().SetTimer(RemoveTimer, [this]()
        {
            if (!IsValid(this)) return;

            DestroyComponent();

            if (!IsValid(GetWorld()))
            {
                GetWorld()->GetTimerManager().ClearTimer(FaceTimer);
            }
        },
        0.8f, false);
}

void UDamageTextWidgetComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetWorld()->GetTimerManager().ClearTimer(FaceTimer);
    GetWorld()->GetTimerManager().ClearTimer(RemoveTimer);
    Super::EndPlay(EndPlayReason);
}