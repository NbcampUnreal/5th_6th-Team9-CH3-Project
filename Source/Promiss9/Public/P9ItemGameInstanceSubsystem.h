// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "P9WeaponData.h"
#include "P9ItemGameInstanceSubsystem.generated.h"

/**
 * 
 */
struct FP9WeaponData;

UCLASS()
class PROMISS9_API UP9ItemGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UP9ItemGameInstanceSubsystem();
	UPROPERTY()
	UDataTable* P9WeaponDataTable;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Weapon|Data")
	FP9WeaponData GetWeaponDataByID(FName ItemID) const;


private:


	
};
