#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "P9GameState.generated.h"


USTRUCT(BlueprintType)
struct FP9WaveData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Wave")
	float WaveEndTime;		// wave 종료 시간
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Wave")
	FString WaveName;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaveChangedSignature, int32, NewWaveIndex, const FString&, NewWaveName);


UCLASS()
class PROMISS9_API AP9GameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AP9GameState();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave")
	TArray<FP9WaveData> WaveSettings;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	float CurrentGameTime;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Wave")
	int32 CurrentWaveIndex;

	UPROPERTY(BlueprintAssignable, Category = "Wave")
	FOnWaveChangedSignature OnWaveChanged;

	void NextWave();

	UFUNCTION()
	void SetCurrentWave(int32 NewWaveIndex);		// wave 변경용
	
};
