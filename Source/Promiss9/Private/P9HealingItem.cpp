// Fill out your copyright notice in the Description page of Project Settings.


#include "P9HealingItem.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"


AP9HealingItem::AP9HealingItem()
{	
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetupAttachment(GetRootComponent()); 

	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collision->SetCollisionObjectType(ECC_WorldDynamic);
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Collision->SetGenerateOverlapEvents(true);

	
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AP9HealingItem::OnItemOverlap);
	//Collision->OnComponentEndOverlap.AddDynamic(this, &AP9HealingItem::OnItemEndOverlap);
}

void AP9HealingItem::OnItemOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	// Implement overlap logic here
	if (AP9Character* PlayerCharacter = Cast<AP9Character>(OtherActor))
	{
		if (APawn* Pawn = Cast<APawn>(OtherActor))
		{
			if(Pawn->IsPlayerControlled())
			{
				ActivateItem(PlayerCharacter);
			}
		}
	}
}

void AP9HealingItem::OnItemEndOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
}

void AP9HealingItem::ActivateItem(AActor* Activator)
{
	if(AP9Character* PlayerCharacter = Cast<AP9Character>(Activator))
	{
		applyHeal(PlayerCharacter, HealAmount);
		DestroyItem();
	}
}

FName AP9HealingItem::GetItemType()const
{
	return ItemType;
}

void AP9HealingItem::DestroyItem()
{
	Destroy();
}

void AP9HealingItem::applyHeal(AActor* Target, float InAmount)
{
	if (!Target || InAmount <= 0.0f)
	{
		return;
	}
	if(AP9Character* PlayerCharacter = Cast<AP9Character>(Target))
	{
		PlayerCharacter->AddHealth(InAmount);
	}
}

