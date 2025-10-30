#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "P9GameState.h"
#include "P9MonsterSpawner.generated.h"

UCLASS()
class PROMISS9_API AP9MonsterSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AP9MonsterSpawner();

protected:
	UPROPERTY(EditAnywhere)
	float MaxSpawnRadius;
	UPROPERTY(EditAnywhere)
	float MinSpawnRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	float SpawnFrequency;

	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<AP9GameState> GameState=nullptr;

	FTimerHandle SpawnTimer;

	int32 CurrentWaveIndex;

	UFUNCTION()
	void WaveStarted(int32 NewWaveIndex, FString NewWaveName);

	bool FindSpawnLocation(FVector& OutSpawnLocation);

	void SpawnEvent();



};
