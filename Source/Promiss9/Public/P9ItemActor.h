// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "P9ItemInterface.h"
#include "P9WeaponData.h"
#include "P9InventoryComponent.h"
#include "P9ItemActor.generated.h"


UCLASS()
class PROMISS9_API AP9ItemActor : public AActor, public IP9ItemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AP9ItemActor();

protected:
	
	virtual void BeginPlay() override;
	//무기 데이터 테이블 가져오는 핸들
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Data")
	FDataTableRowHandle P9WeaponRow;

	// 무기 사거리 열값 가져오기
	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	int32 GetRange(int32 RowNumber) const;

	// 무기 데미지 열값 가져오기
	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	float GetDamage(int32 RowNumber) const;

	// 무기 가격 열값 가져오기
	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	int32 GetPrice(int32 RowNumber) const;

	// 무기 카운트 열값 가져오기
	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	int32 GetCount(int32 RowNumber) const;

	//무기 사속 열값 가져오기
	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	float GetFireSpeed(int32 RowNumber) const;

	//무기 데이터 테이블 행 가져오기
	FP9WeaponData* GetRowData(int32 RowNumber) const;

	bool bOnInventoryWeapon(AActor* Activator, const FP9WeaponData* Row) const;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Root")
	USceneComponent* SceneRoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Mesh")
	UStaticMeshComponent* StaticMesh;
	
	
	UFUNCTION(BlueprintCallable, Category = "Weapon|Action")
	void Fire(int32 RowNumber) const;
	virtual void OnItemOverlap(AActor* OverlapActor) override;
	virtual void OnItemEndOverlap(AActor* OverlapActor) override;
	UFUNCTION(BlueprintCallable, Category = "Weapon|Action")
	virtual void ActivateItem(AActor* Activator, int32 RowNumber) override;
	virtual FName GetItemType(int32 RowNumber) const override;

	virtual void DestroyIterm();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
