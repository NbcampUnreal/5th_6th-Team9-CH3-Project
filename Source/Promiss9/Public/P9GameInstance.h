// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Components/AudioComponent.h"
#include "P9GameInstance.generated.h"

UCLASS()
class PROMISS9_API UP9GameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Audio")
    TWeakObjectPtr<UAudioComponent> CurrentBGM;


    UFUNCTION(BlueprintCallable)
    void ResetGameData();

    UFUNCTION(BlueprintCallable)
    void ClearAllTimersInWorld();

    UFUNCTION(BlueprintCallable)
	void ClearAllActorsInWorld();

public:
    int32 PlayerScore = 0;
    float PlayerHP = 100.f;
};