// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "P9ItemInterface.h"
#include "P9WeaponData.h"
#include "P9ItemActor.generated.h"


UCLASS()
class PROMISS9_API AP9ItemActor : public AActor, public IP9ItemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AP9ItemActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FName ItemType;


	virtual void OnItemOverlap(AActor* OverlapActor) override;
	virtual void OnItemEndOverlap(AActor* OverlapActor) override;
	virtual void ActivateItem(AActor* Activator) override;
	virtual FName GetItemType() const override;

	virtual void DestroyIterm();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
