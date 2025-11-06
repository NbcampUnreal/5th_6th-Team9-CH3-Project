// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "P9GameInstance.generated.h"

UCLASS()
class PROMISS9_API UMyGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void ResetGameData();

public:
    int32 PlayerScore = 0;
    float PlayerHP = 100.f;
};