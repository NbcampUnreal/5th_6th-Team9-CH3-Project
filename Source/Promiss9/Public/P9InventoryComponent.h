// P9InventoryComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "P9InventoryComponent.generated.h"


USTRUCT(BlueprintType)
struct FInventorySlot
{
    GENERATED_BODY()

    /** 무기 ID  */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
    FName WeaponId = NAME_None;

    /** 보유 개수 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    int32 Count = 0;
};

struct FP9WeaponData;
class UDataTable;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROMISS9_API UP9InventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UP9InventoryComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    /** 현재 슬롯 배열 반환 */
    UFUNCTION(BlueprintPure, Category = "Inventory")
    const TArray<FInventorySlot>& GetSlots() const { return Slots; }

    /** 무기 추가. 이미 보유하면 슬롯 추가 없이 수량만 증가 */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool AddWeaponById(FName WeaponId);

    UFUNCTION(BlueprintCallable, Category = "Inventory|Data")
    bool AddWeaponById_Validated(FName WeaponId);

    /** 보유 여부 */
    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool HasWeaponId(FName WeaponId) const;

    UFUNCTION(BlueprintPure, Category = "Inventory")
    int32 GetWeaponCount(FName WeaponId) const;

    /** 슬롯 개수 조정(최소 4칸; 0번은 권총 고정) */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool SetMaxSlots(int32 NewMaxSlots);

    /** 전체 초기화 — 권총 복구 */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void ResetAll();
    // 지금 인벤토리에 들어있는 무기 ID만 뽑기.
    UFUNCTION(BlueprintPure, Category = "Inventory")
    void GetCurrentWeaponIds(TArray<FName>& OutWeaponIds) const;


public:

    UFUNCTION(BlueprintPure, Category = "Inventory|Data")
    bool GetWeaponData(FName WeaponId, FP9WeaponData& OutRow) const;

    /** 슬롯 인덱스의 무기 정보를 DataTable에서 가져오기 */
    UFUNCTION(BlueprintPure, Category = "Inventory|Data")
    bool GetSlotWeaponData(int32 SlotIndex, FP9WeaponData& OutRow) const;

    /** 에디터에서 할당할 무기 DataTable */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory|Data", meta = (AllowPrivateAccess = "true"))
    UDataTable* WeaponDataTable = nullptr;

private:
    /** 슬롯 최대 개수(최소 4; 0번=권총, 1~=추가 무기) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory",
        meta = (ClampMin = "4", AllowPrivateAccess = "true"))
    int32 MaxSlots = 4;

    /** 실제 슬롯 배열 */
    UPROPERTY(VisibleAnywhere, Category = "Inventory")
    TArray<FInventorySlot> Slots;

    /** 0번 슬롯(권총)용 기본 ID */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory|Rule",
        meta = (AllowPrivateAccess = "true"))
    FName DefaultHandgunId = FName("HG");

private:
    void  NormalizeLength();                 // 배열 길이/값 보정 + 권총 보장
    int32 FindEmptySlotIndex_From1() const;  // 1번 인덱스부터 첫 빈칸
    int32 FindSlotIndexById(FName WeaponId) const; // 무기 위치
    void  EnsureDefaultHandgun();            // 0번 권총 보장
};
