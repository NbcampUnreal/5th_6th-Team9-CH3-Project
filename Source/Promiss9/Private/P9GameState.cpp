#include "P9GameState.h"

AP9GameState::AP9GameState()
{
	CurrentGameTime = 0.0f;

	CurrentWaveIndex = -1;
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

	const FString WaveNameString = WaveSettings[CurrentWaveIndex].WaveName;
	if (GEngine)																	// if(GEngine)은 테스트용이기에 추후에 삭제
	{
		GEngine->AddOnScreenDebugMessage(
			-1,        
			5.0f,      
			FColor::Yellow, 
			FString::Printf(TEXT("======= [WAVE %d] %s ========"), NewWaveIndex + 1, *WaveNameString)
		);
	}
	UE_LOG(LogTemp, Warning, TEXT("Wave가 %s 로 변경되었습니다."), *WaveNameString);	//테스트용이기에 추후에 주석처리
	OnWaveChanged.Broadcast(CurrentWaveIndex, WaveNameString);
}