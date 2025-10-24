// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectPtr.h"
#include "P9GameState.h"
#include "GameFramework/GameModeBase.h"
#include "P9GameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROMISS9_API AP9GameMode : public AGameModeBase
{
	GENERATED_BODY()

private:

	UPROPERTY()
	TObjectPtr<AP9GameState> P9GameState;
};
