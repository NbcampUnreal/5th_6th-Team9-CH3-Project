// Fill out your copyright notice in the Description page of Project Settings.


#include "P9InventoryComponent.h"

// Sets default values for this component's properties
UP9InventoryComponent::UP9InventoryComponent()
{
	
	PrimaryComponentTick.bCanEverTick = false;
	NormalizeLength(); // 슬롯배열초기화

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

int32 UP9InventoryComponent::FindEmptySlotIndex() const //빈 슬롯 찾기. 새로운 무기를 넣을 자리 찾기
{
	for (int32 i = 0; i < Slots.Num(); ++i) //for문으로 슬롯을 검사해봄.
	{
		if (Slots[i].WeaponId.IsNone()) // 현재 슬롯이 비어있다면 NAME_NONE
			return i;
	}
	return INDEX_NONE;
}

int32 UP9InventoryComponent::FindSlotIndexById(FName WeaponId) const //가지고 있는 무기 찾기
{
	if (WeaponId.IsNone()) return INDEX_NONE;

	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		if (Slots[i].WeaponId == WeaponId)
			return i;
	}
	return INDEX_NONE;
}

void UP9InventoryComponent::NormalizeLength() //인벤토리 크기 자동 조정
{
	if (Maxslots < 0)
		Maxslots = 0;

	Slots.SetNum(Maxslots);

	for (FInventorySlot& S : Slots)
	{
	if (S.WeaponId.IsNone())
			S.WeaponId = NAME_None;
	}
}
