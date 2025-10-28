// Fill out your copyright notice in the Description page of Project Settings.


#include "P9InventoryComponent.h"

// Sets default values for this component's properties
UP9InventoryComponent::UP9InventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	NormalizeLength();
	// ...
}


// Called when the game starts
void UP9InventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UP9InventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UP9InventoryComponent::NormalizeLength()
{
	if (MaxSlots < 0)
		MaxSlots = 0;

	Slots.SetNum(MaxSlots);

	for (FInventorySlot& S : Slots)
	{
		if (S.WeaponId.IsNone())
			S.WeaponId = NAME_None;
	}
}

int32 UP9InventoryComponent::FindEmptySlotIndex() const
{
	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		if (Slots[i].WeaponId.IsNone())
			return i;
	}
	return INDEX_NONE;
}

int32 UP9InventoryComponent::FindSlotIndexById(FName WeaponId) const
{
	if (WeaponId.IsNone()) return INDEX_NONE;

	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		if (Slots[i].WeaponId == WeaponId)
			return i;
	}
	return INDEX_NONE;
}

bool UP9InventoryComponent::HasWeaponId(FName WeaponId) const
{
	return FindSlotIndexById(WeaponId) != INDEX_NONE;
}