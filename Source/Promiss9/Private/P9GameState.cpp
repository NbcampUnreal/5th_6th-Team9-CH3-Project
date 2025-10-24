#include "P9GameState.h"

AP9GameState::AP9GameState()
{
	CurrentGameTime = 0.0f;

	CurrentWaveIndex = 0;
}

void AP9GameState::NextWave()
{
	SetCurrentWave(CurrentWaveIndex+1);
}

void AP9GameState::SetCurrentWave(int32 NewWaveIndex)
{
	if (CurrentWaveIndex == NewWaveIndex)
	{
		return;
	}

	if (!WaveSettings.IsValidIndex(NewWaveIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetCurrentWave : 마지막 웨이브(%d)에 도달했습니다."), CurrentWaveIndex);
		return;
	}

	CurrentWaveIndex = NewWaveIndex;
	const FString& NewWaveName = WaveSettings[CurrentWaveIndex].WaveName;
	UE_LOG(LogTemp, Warning, TEXT("Wave가 %s 로 변경되었습니다."), *NewWaveName);
	OnWaveChanged.Broadcast(CurrentWaveIndex, NewWaveName);
}