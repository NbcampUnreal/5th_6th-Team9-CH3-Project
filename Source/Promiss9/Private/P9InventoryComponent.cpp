// P9InventoryComponent.cpp
#include "P9InventoryComponent.h"
#include "Engine/DataTable.h"
#include "P9WeaponData.h"

UP9InventoryComponent::UP9InventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    NormalizeLength();
}

void UP9InventoryComponent::BeginPlay()
{
    Super::BeginPlay();
    EnsureDefaultHandgun(); // 시작 시 0번 권총 보장
}

void UP9InventoryComponent::TickComponent(
    float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

/** 권총 보장 */
void UP9InventoryComponent::NormalizeLength()
{
    if (MaxSlots < 4) MaxSlots = 4; // 0=권총, 1~=추가
    const int32 OldNum = Slots.Num();
    Slots.SetNum(MaxSlots);

    // 새로 늘어난 칸 초기화
    for (int32 i = OldNum; i < MaxSlots; ++i)
    {
        Slots[i].WeaponId = NAME_None;
        Slots[i].Count = 0;
    }

    for (int32 i = 0; i < OldNum && i < Slots.Num(); ++i)
    {
        if (!Slots[i].WeaponId.IsNone() && Slots[i].Count <= 0)
        {
            Slots[i].Count = 1;
        }
        if (Slots[i].WeaponId.IsNone())
        {
            Slots[i].Count = 0;
        }
    }

    EnsureDefaultHandgun();
}

/** 1번 인덱스부터 첫 빈칸 */
int32 UP9InventoryComponent::FindEmptySlotIndex_From1() const
{
    for (int32 i = 1; i < Slots.Num(); ++i) // 0은 권총, 1부터만 검색
    {
        if (Slots[i].Count == 0) return i;
    }
    return INDEX_NONE;
}

/** 특정 무기 위치 */
int32 UP9InventoryComponent::FindSlotIndexById(FName WeaponId) const
{
    if (WeaponId.IsNone()) return INDEX_NONE;

    for (int32 i = 0; i < Slots.Num(); ++i)
    {
        if (Slots[i].WeaponId == WeaponId && Slots[i].Count > 0)
            return i;
    }
    return INDEX_NONE;
}

bool UP9InventoryComponent::HasWeaponId(FName WeaponId) const
{
    return FindSlotIndexById(WeaponId) != INDEX_NONE;
}

int32 UP9InventoryComponent::GetWeaponCount(FName WeaponId) const
{
    const int32 Idx = FindSlotIndexById(WeaponId);
    return (Idx != INDEX_NONE) ? Slots[Idx].Count : 0;
}

/** 0번 권총 보장 */
void UP9InventoryComponent::EnsureDefaultHandgun()
{
    if (!Slots.IsValidIndex(0)) return;

    if (DefaultHandgunId.IsNone())
    {
        Slots[0].WeaponId = NAME_None;
        Slots[0].Count = 0;
        return;
    }

    // 권총 고정(없으면 넣고 Count=1)
    Slots[0].WeaponId = DefaultHandgunId;
    if (Slots[0].Count <= 0) Slots[0].Count = 1;
}

/** 무기 추가: 이미 있으면 Count++, 없으면 1~에서 빈칸에 새로 넣기 */
bool UP9InventoryComponent::AddWeaponById(FName WeaponId)
{
    if (WeaponId.IsNone())
        return false;

    // 권총도 구매 가능
    if (WeaponId == DefaultHandgunId)
    {
        if (!Slots.IsValidIndex(0)) return false;

        // 0번은 항상 권총 유지
        Slots[0].WeaponId = DefaultHandgunId;

        // 기본 보유가 1이므로 0이면 1로
        if (Slots[0].Count <= 0) Slots[0].Count = 1;
        else                     Slots[0].Count += 1;

#if !(UE_BUILD_SHIPPING)
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1, 2.0f, FColor::Green,
                FString::Printf(TEXT("[인벤토리] 권총 + 1 → x%d"), Slots[0].Count));
        }
#endif
        return true;
    }

    // 이미 보유 → 슬롯 추가 없이 Count
    if (const int32 OwnedIdx = FindSlotIndexById(WeaponId); OwnedIdx != INDEX_NONE)
    {
        Slots[OwnedIdx].Count += 1;

#if !(UE_BUILD_SHIPPING)
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1, 2.0f, FColor::Green,
                FString::Printf(TEXT("[인벤토리] %s + 1 → x%d"),
                    *WeaponId.ToString(), Slots[OwnedIdx].Count));
        }
#endif
        return true;
    }

    // 처음 획득 → 1~(MaxSlots-1)에서 빈칸 찾기
    const int32 EmptyIdx = FindEmptySlotIndex_From1();
    if (EmptyIdx == INDEX_NONE)
    {
#if !(UE_BUILD_SHIPPING)
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1, 2.0f, FColor::Red, TEXT("[인벤토리] 빈 슬롯이 없습니다."));
        }
#endif
        return false;
    }

    Slots[EmptyIdx].WeaponId = WeaponId;
    Slots[EmptyIdx].Count = 1;

#if !(UE_BUILD_SHIPPING)
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1, 2.0f, FColor::Green,
            FString::Printf(TEXT("[인벤토리] %s 신규 획득 (슬롯 %d)"),
                *WeaponId.ToString(), EmptyIdx));
    }
#endif
    return true;
}

bool UP9InventoryComponent::AddWeaponById_Validated(FName WeaponId)
{
    FP9WeaponData Row;
    if (!GetWeaponData(WeaponId, Row))
        return false;

    return AddWeaponById(WeaponId);
}

/** 슬롯 개수 변경(소지 수보다 작게 줄이면 실패) */
bool UP9InventoryComponent::SetMaxSlots(int32 NewMaxSlots)
{
    if (NewMaxSlots < 4)
        return false;

    // 현재 보유 수(Count>0만 카운트)
    int32 Owned = 0;
    for (const FInventorySlot& S : Slots)
    {
        if (S.Count > 0) ++Owned;
    }

    if (NewMaxSlots < Owned)
        return false;

    MaxSlots = NewMaxSlots;
    NormalizeLength();
    return true;
}

/** 전체 초기화 */
void UP9InventoryComponent::ResetAll()
{
    for (FInventorySlot& S : Slots)
    {
        S.WeaponId = NAME_None;
        S.Count = 0;
    }
    EnsureDefaultHandgun();
}

/** DataTable 연동 */
bool UP9InventoryComponent::GetWeaponData(FName WeaponId, FP9WeaponData& OutRow) const
{
    if (!WeaponId.IsNone() && WeaponDataTable)
    {
        static const FString Ctx(TEXT("Inventory_GetWeaponData"));
        if (const FP9WeaponData* Found =
            WeaponDataTable->FindRow<FP9WeaponData>(WeaponId, Ctx, /*bWarnIfRowMissing*/ false))
        {
            OutRow = *Found; 
                return true;
        }
    }
    return false;
}

bool UP9InventoryComponent::GetSlotWeaponData(int32 SlotIndex, FP9WeaponData& OutRow) const
{
    if (!Slots.IsValidIndex(SlotIndex))
        return false;

    const FName Id = Slots[SlotIndex].WeaponId;
    if (Id.IsNone())
        return false;

    return GetWeaponData(Id, OutRow);
}
