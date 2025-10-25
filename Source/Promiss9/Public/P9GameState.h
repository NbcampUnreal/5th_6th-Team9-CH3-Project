#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "P9Monster.h"
#include "P9GameState.generated.h"


USTRUCT(BlueprintType)
struct FP9WaveData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Wave")
	float WaveEndTime;		// wave 종료 시간
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Wave")
	FString WaveName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave|Spawn")
	TSubclassOf<AP9Monster> Monster;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave|Spawn")
	int32 Population;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave|Spawn")
	float Frequency;
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
