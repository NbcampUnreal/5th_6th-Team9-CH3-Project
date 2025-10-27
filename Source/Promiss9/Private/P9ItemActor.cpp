// Fill out your copyright notice in the Description page of Project Settings.


#include "P9ItemActor.h"

// Sets default values
AP9ItemActor::AP9ItemActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(SceneRoot);

}

// Called when the game starts or when spawned
void AP9ItemActor::BeginPlay()
{
	Super::BeginPlay();

	
}

// Called every frame
void AP9ItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AP9ItemActor::OnItemOverlap(AActor* OverlapActor)
{
}

void AP9ItemActor::OnItemEndOverlap(AActor* OverlapActor)
{
}

void AP9ItemActor::ActivateItem(AActor* Activator)
{
}	

FName AP9ItemActor::GetItemType() const
{
	return FName();
}

void AP9ItemActor::DestroyIterm()
{
	Destroy();
}