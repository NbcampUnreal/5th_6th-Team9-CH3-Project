#include "P9GameMode.h"

void AP9GameMode::BeginPlay()
{
	Super::BeginPlay();

	P9GameState = GetGameState<AP9GameState>();
	if (P9GameState != nullptr)
	{
		GetWorld()->GetTimerManager().SetTimer(WaveTimer, this, &AP9GameMode::UpdateTimer, 1.0f, true);

		P9GameState->SetCurrentWave(0);
	}
}

void AP9GameMode::UpdateTimer()
{
	if (P9GameState == nullptr) return;
	
	P9GameState->CurrentGameTime += 1.0f;
	int32 CurrentWave = P9GameState->CurrentWaveIndex;

	if (P9GameState->WaveSettings.IsValidIndex(CurrentWave))
	{
		float CurrentWaveEndTime = P9GameState->WaveSettings[CurrentWave].WaveEndTime;

		if (P9GameState->CurrentGameTime >= CurrentWaveEndTime)
		{
			P9GameState->NextWave();
		}
	}
}
