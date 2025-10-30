#include "P9InventoryComponent.h"
#include "Engine/DataTable.h"
#include "P9WeaponData.h"

// Sets default values for this component's properties
UP9InventoryComponent::UP9InventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    NormalizeLength();     // 배열 크기/초기값 보정
}

void UP9InventoryComponent::BeginPlay()
{
    Super::BeginPlay();
    EnsureDefaultHandgun(); // 시작 시 1번 권총 보장
}

void UP9InventoryComponent::TickComponent(
    float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// ─────────────────────────────────────
// 내부: 배열 길이/값 보정 + 1번(인덱스0) 권총 보장 호출
// ─────────────────────────────────────
void UP9InventoryComponent::NormalizeLength()
{
    if (MaxSlots < 4) MaxSlots = 4; // 최소 4칸 보장 (1:권총, 2~4:추가)
    const int32 OldNum = Slots.Num();
    Slots.SetNum(MaxSlots);

    // 새로 늘어난 칸 초기화
    for (int32 i = OldNum; i < MaxSlots; ++i)
    {
        Slots[i].WeaponId = NAME_None;
    }
    // 권총 보장(헤더/리셋에서도 호출)
    EnsureDefaultHandgun();
}

// ─────────────────────────────────────
// 내부: 2~4번(인덱스 1~) 범위에서 첫 빈칸 찾기
// ─────────────────────────────────────
int32 UP9InventoryComponent::FindEmptySlotIndex_From1() const
{
    for (int32 i = 1; i < Slots.Num(); ++i) // 인덱스 1부터
    {
        if (Slots[i].WeaponId.IsNone())
            return i;
    }
    return INDEX_NONE;
}

// ─────────────────────────────────────
// 내부: 특정 무기 ID가 담긴 슬롯 인덱스
// ─────────────────────────────────────
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

// ─────────────────────────────────────
// 보유 여부 확인
// ─────────────────────────────────────
bool UP9InventoryComponent::HasWeaponId(FName WeaponId) const
{
    return FindSlotIndexById(WeaponId) != INDEX_NONE;
}

// ─────────────────────────────────────
// 내부: 1번 슬롯 권총 기본 보장
//  - 비어 있거나 다른 무기가 있으면 권총으로 세팅
// ─────────────────────────────────────
void UP9InventoryComponent::EnsureDefaultHandgun()
{
    if (!Slots.IsValidIndex(0)) return;

    if (DefaultHandgunId.IsNone())
    {
        // 권총 ID가 비어 있으면 그냥 비워둠
        Slots[0].WeaponId = NAME_None;
        return;
    }

    if (Slots[0].WeaponId != DefaultHandgunId)
    {
        Slots[0].WeaponId = DefaultHandgunId;
    }
}

// ─────────────────────────────────────
// 무기 추가 (권총 제외, 2~4번 슬롯에만 영구 추가)
//  - 중복 무기 추가 불가
// ─────────────────────────────────────
bool UP9InventoryComponent::AddWeaponById(FName WeaponId)
{
    if (WeaponId.IsNone())
        return false;

    // 1번 슬롯 권총은 수동 추가/변경 불가
    if (WeaponId == DefaultHandgunId)
    {
#if !(UE_BUILD_SHIPPING)
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan,
                TEXT("[Inventory] 권총은 기본 소지이므로 AddWeaponById로 추가하지 않습니다."));
        }
#endif
        return false;
    }

    if (HasWeaponId(WeaponId))
        return false;

    const int32 EmptyIdx = FindEmptySlotIndex_From1(); // 2~4번만 검색
    if (EmptyIdx == INDEX_NONE)
        return false;

    Slots[EmptyIdx].WeaponId = WeaponId;

#if !(UE_BUILD_SHIPPING)
    if (GEngine)
    {
        FString Msg = FString::Printf(TEXT("[인벤토리] 무기 추가됨: %s (슬롯 %d)"),
            *WeaponId.ToString(), EmptyIdx + 1);
        GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Green, Msg);
    }
#endif
    return true;
}

// ─────────────────────────────────────
// 슬롯 개수 변경 (최소 4칸, 소지 수보다 작게 줄이면 실패)
// ─────────────────────────────────────
bool UP9InventoryComponent::SetMaxSlots(int32 NewMaxSlots)
{
    if (NewMaxSlots < 4)
        return false;

    // 현재 보유 수(빈칸 제외)
    int32 Owned = 0;
    for (const FInventorySlot& S : Slots)
    {
        if (!S.WeaponId.IsNone())
            ++Owned;
    }

    if (NewMaxSlots < Owned)
        return false;

    MaxSlots = NewMaxSlots;
    NormalizeLength();
    return true;
}

// ─────────────────────────────────────
// 전체 초기화(사망/재시작)
//  - 전체 비우고 권총 복구
// ─────────────────────────────────────
void UP9InventoryComponent::ResetAll()
{
    for (FInventorySlot& S : Slots)
        S.WeaponId = NAME_None;

    EnsureDefaultHandgun();
}

// ─────────────────────────────────────
// DataTable 연동
// ─────────────────────────────────────
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

bool UP9InventoryComponent::AddWeaponById_Validated(FName WeaponId)
{
    FP9WeaponData Row;
    if (!GetWeaponData(WeaponId, Row))
        return false; // 정의되지 않은 ID

    return AddWeaponById(WeaponId);
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
