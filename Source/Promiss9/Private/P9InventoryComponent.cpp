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
// 슬롯 배열 길이/값 보정
// Slots 크기를 MaxSlots로 맞춤
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
//첫 번째 빈 슬롯 인덱스
int32 UP9InventoryComponent::FindEmptySlotIndex() const
{
	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		if (Slots[i].WeaponId.IsNone())
			return i;
	}
	return INDEX_NONE;
}
//무기ID가 담긴 슬롯 인덱스
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
// 보유 여부 확인
bool UP9InventoryComponent::HasWeaponId(FName WeaponId) const
{
	return FindSlotIndexById(WeaponId) != INDEX_NONE;
}
//무기 추가(영구 장착),  중복 금지
bool UP9InventoryComponent::AddWeaponById(FName WeaponId)
{
	if (WeaponId.IsNone()) return false;
	if (HasWeaponId(WeaponId)) return false;

	const int32 EmptyIdx = FindEmptySlotIndex();
	if (EmptyIdx == INDEX_NONE)
		return false;

	Slots[EmptyIdx].WeaponId = WeaponId;
	return true;
}
// 슬롯 개수 변경
bool UP9InventoryComponent::SetMaxSlots(int32 NewMaxSlots)
{
	if (NewMaxSlots < 0) return false;

	// 현재 보유 무기 개수
	int32 Owned = 0;
	for (const FInventorySlot& S : Slots)
	{
		if (!S.WeaponId.IsNone())
			Owned++;
	}

	// 보유 수보다 작게 줄이면 실패
	if (NewMaxSlots < Owned)
		return false;

	MaxSlots = NewMaxSlots;
	NormalizeLength();
	return true;
}
// 전체 초기화(죽음/재시작 시 호출)
void UP9InventoryComponent::ResetAll()
{
	for (FInventorySlot& S : Slots)
	{
		S.WeaponId = NAME_None;
	}
}