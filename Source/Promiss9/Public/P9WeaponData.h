// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "P9WeaponData.generated.h"


UENUM(BlueprintType)
enum class EItemType : uint8
{
	Equipment UMETA(DisplayName = "Equipment"),
	collect UMETA(DisplayName = "Collect")
};

USTRUCT(BlueprintType)
struct FP9WeaponData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FName ItemID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	UTexture2D* ItemIcon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 Range;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 Count;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 Price;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float FireSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<class AP9ItemActor> WeaponActorClass;

};
