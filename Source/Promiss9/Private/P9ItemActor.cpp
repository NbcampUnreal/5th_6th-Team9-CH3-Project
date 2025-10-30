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

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UP9ItemGameInstanceSubsystem* ItemSubsystem = GameInstance->GetSubsystem<UP9ItemGameInstanceSubsystem>())
		{
			FP9WeaponData WeaponData = ItemSubsystem->GetWeaponDataByID(P9WeaponRow.RowName);
			Damage = WeaponData.Damage;
			Range = WeaponData.Range;
			FireSpeed = WeaponData.FireSpeed;
			Price = WeaponData.Price;
			Count = WeaponData.Count;
		}
		
	}
	
}
/*

void AP9ItemActor::SetRange(float Range)
{
	CurrentRange = FMath::Max(0, Range);;
}

void AP9ItemActor::SetDamage(float Damage)
{
	CurrentDamage = FMath::Max(0.0f, Damage);
}

void AP9ItemActor::SetFireSpeed(float FireSpeed)
{
	CurrentFireSpeed = FMath::Max(0.0f, FireSpeed);
}

float AP9ItemActor::GetCurrentRange()const
{
	return CurrentRange;
}

float AP9ItemActor::GetCurrentDamage()const
{
	return CurrentDamage;
}

float AP9ItemActor::GetCurrentFireSpeed()const
{
	return CurrentFireSpeed;
}*/


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

/*FP9WeaponData* AP9ItemActor::GetRowData(int32 RowNumber) const
{
	if (!P9WeaponRow.DataTable)
	{
		return nullptr;
	}
	else
	{
		return P9WeaponRow.DataTable->FindRow<FP9WeaponData>(FName(*FString::FromInt(RowNumber)), TEXT("AP9ItemActor::GetRowData"));
	}
}*/

/*void AP9ItemActor::Fire(int32 RowNumber) const
{
	FP9WeaponData* Row = GetRowData(RowNumber);
	if (Row)
	{
		UE_LOG(LogTemp, Warning, TEXT("Firing weapon: %s, Damage: %f, Range: %d, FireSpeed: %f"), *Row->ItemID.ToString(), Row->Damage, Row->Range, Row->FireSpeed);
	}

}*/

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

/*void AP9ItemActor::ActivateItem(AActor* Activator, int32 RowNumber)
{
	CurrentFromRow(RowNumber);
	Fire(RowNumber);
	/*FP9WeaponData* SelectedRow = GetRowData(RowNumber);
	if (!SelectedRow)
	{
		return;
	}
	if(!bOnInventoryWeapon(Activator, SelectedRow))z
	{
		return;
	}
	if (bOnInventoryWeapon && SelectedRow)
	{
		return Fire(RowNumber);
	}
	return;
}*/

float AP9ItemActor::GetRange() const
{
	return Range;
}

float AP9ItemActor::GetDamage() const
{
	return Damage;
}

int32 AP9ItemActor::GetPrice() const
{
	return Price;
}

int32 AP9ItemActor::GetCount() const
{
	return Count;
}

float AP9ItemActor::GetFireSpeed() const
{
	return FireSpeed;
}

float AP9ItemActor::CurrentRange() const
{
	return GetRange();
}

float AP9ItemActor::CurrentDamage() const
{
	return GetDamage();
}

float AP9ItemActor::CurrentFireSpeed() const
{
	return GetFireSpeed();
}

FName AP9ItemActor::GetItemType(int32 RowNumber) const
{
	return FName(FString::FromInt(RowNumber));
}

void AP9ItemActor::DestroyItem()
{
	Destroy();
}
