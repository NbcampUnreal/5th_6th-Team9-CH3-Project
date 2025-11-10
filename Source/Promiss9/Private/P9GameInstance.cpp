// Fill out your copyright notice in the Description page of Project Settings.


#include "P9GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UP9GameInstance::ResetGameData()
{
    PlayerScore = 0;
    PlayerHP = 100.f;
    ClearAllTimersInWorld();
    ClearAllActorsInWorld();

}


void UP9GameInstance::ClearAllTimersInWorld()
{
    if (UWorld* World = GetWorld())
    {
        // 현재 월드의 모든 액터 순회
        for (TActorIterator<AActor> It(World); It; ++It)
        {
            AActor* Actor = *It;
            if (Actor)
            {
                World->GetTimerManager().ClearAllTimersForObject(Actor);
            }
        }

        // GameInstance 자기 자신 타이머도 정리
        World->GetTimerManager().ClearAllTimersForObject(this);
    }
}

void UP9GameInstance::ClearAllActorsInWorld()
{
    if (UWorld* World = GetWorld())
    {
        for (TActorIterator<AActor> It(World); It; ++It)
        {
            AActor* Actor = *It;
            if (Actor && IsValid(Actor) && !Actor->IsActorBeingDestroyed())
            {
                Actor->SetLifeSpan(0.2f);
            }
        }

        // 타이머도 같이 정리
        World->GetTimerManager().ClearAllTimersForObject(this);
    }
}