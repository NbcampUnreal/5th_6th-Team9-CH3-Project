// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "P9InventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	FName WeaponId = NAME_None; // 무기 이름
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROMISS9_API UP9InventoryComponent : public UActorComponent
{
	GENERATED_BODY()


public:	
	// Sets default values for this component's properties
	UP9InventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// 인벤토리의 슬롯 개수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = "0"), meta = (AllowPrivateAccess = "true"))
	int32 MaxSlots = 4;

	// 실제 슬롯 배열
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TArray<FInventorySlot> Slots;

public:
    //// 현재 슬롯 배열 가져오기
    //UFUNCTION(BlueprintPure, Category = "Inventory")
    //const TArray<FInventorySlot>& GetSlots() const { return Slots; }

    //// 무기 추가 
    //UFUNCTION(BlueprintCallable, Category = "Inventory")
    //bool AddWeaponById(FName WeaponId);

    //// 보유 여부 확인
    //UFUNCTION(BlueprintPure, Category = "Inventory")
    //bool HasWeaponId(FName WeaponId) const;

    //// 슬롯 개수 조정
    //UFUNCTION(BlueprintCallable, Category = "Inventory")
    //bool SetMaxSlots(int32 NewMaxSlots);

    //// 전체 초기화
    //UFUNCTION(BlueprintCallable, Category = "Inventory")
    //void ResetAll();

private:
    void NormalizeLength(); // 내부 정리 함수
    int32 FindEmptySlotIndex() const;
    int32 FindSlotIndexById(FName WeaponId) const;

};
