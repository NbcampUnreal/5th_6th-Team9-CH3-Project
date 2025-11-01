// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "P9ItemActor.h"
#include "P9Character.h"
#include "P9HealingItem.generated.h"

class UPrimitiveComponent;
class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class PROMISS9_API AP9HealingItem : public AP9ItemActor
{
	GENERATED_BODY()


public:
	AP9HealingItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealingItem")
	FName ItemType = FName("Healing");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealingItem|Component")
	USphereComponent* Collision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealingItem|Amount")
	float HealAmount = 40.0f;

private:
	
	UFUNCTION()
	virtual void OnItemOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;
	UFUNCTION()
	virtual void OnItemEndOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex) override;

	virtual void ActivateItem(AActor* Activator);

	virtual FName GetItemType() const override;

	virtual void DestroyItem() override;

	void applyHeal(AActor* Target, float InAmount);


};
