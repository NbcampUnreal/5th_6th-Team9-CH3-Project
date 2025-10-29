// P9InventoryComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "P9InventoryComponent.generated.h"

//  슬롯:WeaponId(FName) 
USTRUCT(BlueprintType)
struct FInventorySlot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
    FName WeaponId = NAME_None;   // 비어있으면 NAME_None
};

//  전방 선언
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

public:
    /** 현재 슬롯 배열을 그대로 반환 */
    UFUNCTION(BlueprintPure, Category = "Inventory")
    const TArray<FInventorySlot>& GetSlots() const { return Slots; }

    /** 무기 추가 */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool AddWeaponById(FName WeaponId);

    /** 보유 여부 확인 */
    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool HasWeaponId(FName WeaponId) const;

    /** 슬롯 개수 조정 */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool SetMaxSlots(int32 NewMaxSlots);

    /** 전체 초기화(사망/재시작) */
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void ResetAll();

    // ── DataTable 연동
public:
    UFUNCTION(BlueprintPure, Category = "Inventory|Data")
    bool  GetWeaponData(FName WeaponId, FP9WeaponData& OutRow) const;

    /** DataTable에 존재하는 무기만 추가 */
    UFUNCTION(BlueprintCallable, Category = "Inventory|Data")
    bool AddWeaponById_Validated(FName WeaponId);

    /** 슬롯 인덱스의 무기 정보를 DataTable에서 가져오기 */
    UFUNCTION(BlueprintPure, Category = "Inventory|Data")
    bool GetSlotWeaponData(int32 SlotIndex, FP9WeaponData& OutRow) const;

    /** 에디터에서 할당할 무기 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory|Data",
        meta = (AllowPrivateAccess = "true"))
    UDataTable* WeaponDataTable = nullptr;

    //  내부 구현
private:
    /** 슬롯 최대 개수 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory",
        meta = (ClampMin = "0", AllowPrivateAccess = "true"))
    int32 MaxSlots = 4;

    /** 실제 슬롯 배열 */
    UPROPERTY(VisibleAnywhere, Category = "Inventory")
    TArray<FInventorySlot> Slots;

private:
    void   NormalizeLength();                 // 배열 길이/값 보정
    int32  FindEmptySlotIndex() const;        // 첫 빈칸
    int32  FindSlotIndexById(FName WeaponId) const; //무기 위치
};
