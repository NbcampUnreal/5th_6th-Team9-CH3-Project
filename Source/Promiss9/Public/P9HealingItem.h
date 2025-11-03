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
	float HealAmount = 0.2f;
	//드롭 확률
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealingItem|Drop")
	float DropChance = 0.1f;
	//드롭위치 오프셋(위치)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealingItem|Drop")
	FVector DropOffset = FVector(0.0f, 0.0f, 20.0f);
	//드롭시 뛰어오르는 위치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealingItem|Drop")
	float DropImpulse = 200.f;
	//드롭 가능 여부
	UPROPERTY(EditDefaultsOnly, Category = "HealingItem|Drop")
	bool bEligibleForDrop = true;
	UPROPERTY(EditDefaultsOnly)
	float DropWeight = 1.0f;
	//아이템 드롭 함수
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static AP9HealingItem* SpawnHealingItem(const UObject* WorldContextObject, const AActor* SourceActor);

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
