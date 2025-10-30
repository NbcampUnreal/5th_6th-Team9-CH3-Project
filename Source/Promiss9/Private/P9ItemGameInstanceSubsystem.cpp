// Fill out your copyright notice in the Description page of Project Settings.


#include "P9ItemGameInstanceSubsystem.h"


UP9ItemGameInstanceSubsystem::UP9ItemGameInstanceSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> WeaponDataTableFinder(TEXT("/Script/Engine.DataTable'/Game/Blueprints/System/DT_P9WeaponDataTable.DT_P9WeaponDataTable'"));
	if (WeaponDataTableFinder.Succeeded())
	{
		P9WeaponDataTable = WeaponDataTableFinder.Object;
	}
}
void UP9ItemGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

FP9WeaponData UP9ItemGameInstanceSubsystem::GetWeaponDataByID(FName ItemID) const
{
	if(ItemID.IsValid()&& P9WeaponDataTable)
	{
		if(FP9WeaponData* ItemData = P9WeaponDataTable->FindRow<FP9WeaponData>(ItemID,TEXT("GetWeaponDataByID")))
		{
			UE_LOG(LogTemp, Warning, TEXT("Found ItemID"));
			return *ItemData;
		}
	}
	return FP9WeaponData{};
}

void UP9ItemGameInstanceSubsystem::Deinitialize()
{
	Super::Deinitialize();
}