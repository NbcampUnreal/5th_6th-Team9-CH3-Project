// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "P9ItemInterface.h"
#include "P9WeaponData.h"
#include "P9InventoryComponent.h"
#include "P9ItemGameInstanceSubsystem.h"
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Data")
	FDataTableRowHandle P9WeaponRow;



	
	//FP9WeaponData* GetRowData(int32 RowNumber) const;

	bool bOnInventoryWeapon(AActor* Activator, const FP9WeaponData* Row) const;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Root")
	USceneComponent* SceneRoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Mesh")
	UStaticMeshComponent* StaticMesh;
	
	
	//UFUNCTION(BlueprintCallable, Category = "Weapon|Action")
	//void Fire(int32 RowNumber) const;
	virtual void OnItemOverlap(AActor* OverlapActor) override;
	virtual void OnItemEndOverlap(AActor* OverlapActor) override;
	UFUNCTION(BlueprintCallable, Category = "Weapon|Action")
	//virtual void ActivateItem(AActor* Activator, int32 RowNumber) override;
	virtual FName GetItemType(int32 RowNumber) const override;

	virtual void DestroyItem();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Base")
	float Damage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Base")
	float Range;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Base")
	float FireSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Base")
	int32 Price;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Base")
	int32 Count;


	

	//void  SetRange(float Range);

	//void SetDamage(float Damage);

	//void SetFireSpeed(float FireSpeed);


	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	float GetRange() const;


	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	float GetDamage() const;


	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	int32 GetPrice() const;


	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	int32 GetCount() const;


	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	float GetFireSpeed() const;



	UFUNCTION(BlueprintCallable, Category = "Weapon|State")
	float CurrentRange() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon|State")
	float CurrentDamage() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon|State")
	float CurrentFireSpeed () const;

	/*UFUNCTION(BlueprintCallable, Category = "Weapon|State")
	float GetCurrentRange() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon|State")
	float GetCurrentDamage() const;
	UFUNCTION(BlueprintCallable, Category = "Weapon|State")
	float GetCurrentFireSpeed() const;*/
};
