// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "DamageTextWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class PROMISS9_API UDamageTextWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	FTimerHandle FaceTimer;
	FTimerHandle RemoveTimer;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};
