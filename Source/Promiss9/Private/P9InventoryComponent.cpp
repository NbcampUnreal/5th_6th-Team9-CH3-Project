// P9InventoryComponent.cpp
#include "P9InventoryComponent.h"
#include "Engine/DataTable.h"     
#include "P9WeaponData.h"          

// Sets default values for this component's properties
UP9InventoryComponent::UP9InventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    NormalizeLength(); // 시작 시 배열 길이 보정
}

void UP9InventoryComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UP9InventoryComponent::TickComponent(
    float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// 슬롯 배열 길이/값 보정

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

// 첫 번째 빈 슬롯 인덱스 
int32 UP9InventoryComponent::FindEmptySlotIndex() const
{
    for (int32 i = 0; i < Slots.Num(); ++i)
    {
        if (Slots[i].WeaponId.IsNone())
            return i;
    }
    return INDEX_NONE;
}

// 특정 무기 ID가 담긴 슬롯 인덱스
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

// 무기 추가
bool UP9InventoryComponent::AddWeaponById(FName WeaponId)
{
    if (WeaponId.IsNone())
        return false;
    if (HasWeaponId(WeaponId))
        return false;

    const int32 EmptyIdx = FindEmptySlotIndex();
    if (EmptyIdx == INDEX_NONE)
        return false;

    Slots[EmptyIdx].WeaponId = WeaponId;

    //  디버그 메시지 추가
    FString Msg = FString::Printf(TEXT("[인벤토리] 무기 추가됨: %s (슬롯 %d)"), *WeaponId.ToString(), EmptyIdx);
    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, Msg);

    return true;
}

// 슬롯 개수 변경 
bool UP9InventoryComponent::SetMaxSlots(int32 NewMaxSlots)
{
    if (NewMaxSlots < 0)
        return false;

    // 현재 보유 수
    int32 Owned = 0;
    for (const FInventorySlot& S : Slots)
    {
        if (!S.WeaponId.IsNone())
            ++Owned;
    }

    // 보유 수보다 작게 줄이면 실패
    if (NewMaxSlots < Owned)
        return false;

    MaxSlots = NewMaxSlots;
    NormalizeLength();
    return true;
}

// 전체 초기화(사망/재시작)
void UP9InventoryComponent::ResetAll()
{
    for (FInventorySlot& S : Slots)
        S.WeaponId = NAME_None;
}

// DataTable 연동 
bool UP9InventoryComponent::GetWeaponData(FName WeaponId, FP9WeaponData& OutRow) const
{
    if (!WeaponId.IsNone() && WeaponDataTable)
    {
        static const FString Ctx(TEXT("Inventory_GetWeaponData"));
        if (const FP9WeaponData* Found =
            WeaponDataTable->FindRow<FP9WeaponData>(WeaponId, Ctx, /*bWarnIfRowMissing*/ false))
        {
            OutRow = *Found; // 값 복사
            return true;
        }
    }
    return false;
}

//  DataTable에 존재하는 ID만 허용
bool UP9InventoryComponent::AddWeaponById_Validated(FName WeaponId)
{
    FP9WeaponData Row;
    if (!GetWeaponData(WeaponId, Row))
        return false;                 // 정의되지 않은 ID

    return AddWeaponById(WeaponId);
}

// 슬롯 인덱스의 무기 정보를 DataTable에서 가져오기
bool UP9InventoryComponent::GetSlotWeaponData(int32 SlotIndex, FP9WeaponData& OutRow) const
{
    if (!Slots.IsValidIndex(SlotIndex))
        return false;

    const FName Id = Slots[SlotIndex].WeaponId;
    if (Id.IsNone())
        return false;

    return GetWeaponData(Id, OutRow);
}
