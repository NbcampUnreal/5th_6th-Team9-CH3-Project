// Fill out your copyright notice in the Description page of Project Settings.


#include "P9GameInstance.h"
#include "Kismet/GameplayStatics.h"

void UMyGameInstance::ResetGameData()
{
    PlayerScore = 0;
    PlayerHP = 100.f;

}