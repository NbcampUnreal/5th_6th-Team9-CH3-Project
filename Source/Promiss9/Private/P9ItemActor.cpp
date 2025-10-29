// Fill out your copyright notice in the Description page of Project Settings.


#include "P9ItemActor.h"

// Sets default values
AP9ItemActor::AP9ItemActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(SceneRoot);

}

// Called when the game starts or when spawned
void AP9ItemActor::BeginPlay()
{
	Super::BeginPlay();

	
}

// Called every frame
void AP9ItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AP9ItemActor::OnItemOverlap(AActor* OverlapActor)
{

}

void AP9ItemActor::OnItemEndOverlap(AActor* OverlapActor)
{

}

FP9WeaponData* AP9ItemActor::GetRowData(int32 RowNumber) const
{
	if (!P9WeaponRow.DataTable)
	{
		return nullptr;
	}
	else
	{
		return P9WeaponRow.DataTable->FindRow<FP9WeaponData>(FName(*FString::FromInt(RowNumber)), TEXT("AP9ItemActor::GetRowData"));
	}
}

void AP9ItemActor::Fire(int32 RowNumber) const
{
	FP9WeaponData* Row = GetRowData(RowNumber);
	if (Row)
	{
		UE_LOG(LogTemp, Warning, TEXT("Firing weapon: %s, Damage: %f, Range: %d, FireSpeed: %f"), *Row->ItemID.ToString(), Row->Damage, Row->Range, Row->FireSpeed);
	}

}

bool AP9ItemActor::bOnInventoryWeapon(AActor* Activator, const FP9WeaponData* Row) const
{
	if (!Activator && !Row)
	{
		return false;
	}
	
	UP9InventoryComponent* InventoryComp = Activator->FindComponentByClass<UP9InventoryComponent>();
	if (!InventoryComp)
	{
		return false;
	}
	return InventoryComp->HasWeaponId(Row->ItemID);
}

void AP9ItemActor::ActivateItem(AActor* Activator, int32 RowNumber)
{
	FP9WeaponData* SelectedRow = GetRowData(RowNumber);
	if (!SelectedRow)
	{
		return;
	}
	if(!bOnInventoryWeapon(Activator, SelectedRow))
	{
		return;
	}
	Fire(RowNumber);
}	

int32 AP9ItemActor::GetRange(int32 RowNumber) const
{
	FP9WeaponData* Row = GetRowData(RowNumber);
	if (Row)
	{
		return Row->Range;
	}
	return 0;
}

float AP9ItemActor::GetDamage(int32 RowNumber) const
{
	FP9WeaponData* Row = GetRowData(RowNumber);
	if (Row)
	{
		return Row->Damage;
	}
	return 0.f;
}

int32 AP9ItemActor::GetPrice(int32 RowNumber) const
{
	FP9WeaponData* Row = GetRowData(RowNumber);
	if (Row)
	{
		return Row->Price;
	}
	return 0;
}

int32 AP9ItemActor::GetCount(int32 RowNumber) const
{
	FP9WeaponData* Row = GetRowData(RowNumber);
	if (Row)
	{
		return Row->Count;
	}
	return 0;
}

float AP9ItemActor::GetFireSpeed(int32 RowNumber) const
{
	FP9WeaponData* Row = GetRowData(RowNumber);
	if (Row)
	{
		return Row->FireSpeed;
	}
	return 0.f;
}

FName AP9ItemActor::GetItemType(int32 RowNumber) const
{
	return FName(FString::FromInt(RowNumber));
}

void AP9ItemActor::DestroyIterm()
{
	Destroy();
}