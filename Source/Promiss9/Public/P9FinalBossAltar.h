#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "P9Monster.h"
#include "P9FinalBossAltar.generated.h"

UCLASS()
class PROMISS9_API AP9FinalBossAltar : public AActor
{
	GENERATED_BODY()
	
public:	

	AP9FinalBossAltar();


protected:
	UPROPERTY(VisibleAnywhere, Category ="Altar")
	TObjectPtr<USphereComponent> Trigger=nullptr;

	UPROPERTY(EditAnywhere, Category = "Altar")
	TObjectPtr<USceneComponent> BossSpawnPoint=nullptr;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> RootScene = nullptr;

	UPROPERTY(EditAnywhere, Category = "Altar")
	TSubclassOf<AP9Monster> FinalBoss=nullptr;


	virtual void BeginPlay() override;




public:	

	UPROPERTY(EditAnywhere, Category = "Altar")
	float InteractionDuration = 3.0f;

	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OverlapEnd(UPrimitiveComponent* OvelappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void InteractionTimerComplete();

};
