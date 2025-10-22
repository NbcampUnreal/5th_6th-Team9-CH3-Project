// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "P9ItemInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UP9ItemInterface : public UInterface
{
	GENERATED_BODY()
};

class PROMISS9_API IP9ItemInterface
{
	GENERATED_BODY()

	
public:
	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "아이템 정보")
	//FName GetWeaponID() const;

	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "장착 위치")
	//FName GetEquipSocket() const;
};
