// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "P9Monster.h"
#include "P9MiddleDragonMonster.generated.h"

UCLASS()
class PROMISS9_API AP9MiddleDragonMonster : public AP9Monster
{
	GENERATED_BODY()

public:
	AP9MiddleDragonMonster();

protected:
	virtual void BeginPlay() override;

};
