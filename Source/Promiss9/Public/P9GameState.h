#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "P9Monster.h"
#include "P9GameState.generated.h"

USTRUCT(BlueprintType)
struct FSpawnData
{
	GENERATED_BODY()

	FSpawnData()
	{
		MonsterClass = nullptr;
		SpawnCount = 0;
	}
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AP9Monster> MonsterClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SpawnCount;
};
USTRUCT(BlueprintType)
struct FP9WaveData
{
	GENERATED_BODY()
	
	FP9WaveData()
	{
		WaveEndTime = 0.0f;
	}
public:
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Wave")
	float WaveEndTime;		// wave 종료 시간
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Wave")
	FString WaveName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FSpawnData> SpawnList;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave|Boss")
	TSubclassOf<AP9Monster> MidBoss;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaveChangedSignature, int32, NewWaveIndex, FString, NewWaveName);


UCLASS()
class PROMISS9_API AP9GameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AP9GameState();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave")
	TArray<FP9WaveData> WaveSettings;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurrentGameTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 CurrentWaveIndex;

	UPROPERTY(BlueprintAssignable, Category = "Wave")
	FOnWaveChangedSignature OnWaveChanged;

	void NextWave();

	void SetCurrentWave(int32 NewWaveIndex);		// wave 변경용
	
};
