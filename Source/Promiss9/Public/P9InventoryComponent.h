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
	FName WeaponId = NAME_None; // 무기 이름 넣기.
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROMISS9_API UP9InventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UP9InventoryComponent();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = "0"), meta=(AllowPrivateAccess="true"))
	int32 Maxslots = 4; // 인벤토리 슬롯 개수

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TArray<FInventorySlot> Slots;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

//public:
//	//현재 슬롯의 배열을 가져옴
//	UFUNCTION(BlueprintPure, Category = "Inventory")
//	const TArray<FInventorySlot>& GetSlots() const { return Slots; }
//
//	//무기 추가
//	UFUNCTION(BlueprintCallable, Category = "Inventory")
//	bool AddWeaponById(FName WeaponId);
//
//	//보유 했는지 확인
//	UFUNCTION(BlueprintPure, Category = "Inventory")
//	bool HasWeaponId(FName WeaponId) const;
//
//	//슬롯 개수 조정
//	UFUNCTION(BlueprintCallable, Category = "Inventory")
//	bool SetMaxSlots(int32 NewMaxSlots);
//
//	//전체 초기화
//	UFUNCTION(BlueprintCallable, Category = "Inventory")
//	void ResetAll();

private:
	void NormalizeLength();
	int32 FindEmptySlotIndex() const;
	int32 FindSlotIndexById(FName WeaponId) const;

};
