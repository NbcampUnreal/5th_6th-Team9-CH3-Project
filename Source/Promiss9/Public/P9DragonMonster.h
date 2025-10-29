// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "P9Monster.h"
#include "P9DragonMonster.generated.h"

UCLASS()
class PROMISS9_API AP9DragonMonster : public AP9Monster
{
	GENERATED_BODY()

public:
	AP9DragonMonster();

protected:
	virtual void BeginPlay() override;
	
};
