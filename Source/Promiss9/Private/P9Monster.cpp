// Fill out your copyright notice in the Description page of Project Settings.


#include "P9Monster.h"

// Sets default values
AP9Monster::AP9Monster()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AP9Monster::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AP9Monster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AP9Monster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

