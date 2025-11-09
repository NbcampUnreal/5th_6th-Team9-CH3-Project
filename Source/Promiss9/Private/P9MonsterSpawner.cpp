#include "P9MonsterSpawner.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"


AP9MonsterSpawner::AP9MonsterSpawner()
{

	PrimaryActorTick.bCanEverTick = false;
	MinSpawnRadius = 1000.0f;
	MaxSpawnRadius = 5000.0f;
	SpawnFrequency = 3.0f;
	CurrentWaveIndex = -1;

}

void AP9MonsterSpawner::BeginPlay()
{
	Super::BeginPlay();
	GameState = GetWorld()->GetGameState<AP9GameState>();
	if (GameState != nullptr)
	{
		GameState->OnWaveChanged.AddDynamic(this, &AP9MonsterSpawner::WaveStarted);
	}
}

void AP9MonsterSpawner::WaveStarted(int32 NewWaveIndex, FString NewWaveName)
{
	if (GameState == nullptr) return;

	const int32 PreWaveIndex = NewWaveIndex - 1;
	if (GameState->WaveSettings.IsValidIndex(PreWaveIndex))
	{
		const FP9WaveData PreWaveData = GameState->WaveSettings[PreWaveIndex];

		UE_LOG(LogTemp, Warning, TEXT("MidBoss: %s"), *PreWaveData.MidBoss->GetName());

		if (PreWaveData.MidBoss != nullptr)
		{
			FVector BossSpawnLocation;
			if (FindSpawnLocation(BossSpawnLocation))
			{
				GetWorld()->SpawnActor<AP9Monster>(PreWaveData.MidBoss, BossSpawnLocation, FRotator::ZeroRotator);
			}
			else {
				GetWorld()->SpawnActor<AP9Monster>(PreWaveData.MidBoss, MiddleBossSpawnCenter, FRotator::ZeroRotator);
			}
		}
	}
	CurrentWaveIndex = NewWaveIndex;

	if (!GetWorld()->GetTimerManager().IsTimerActive(SpawnTimer))
	{
		GetWorld()->GetTimerManager().SetTimer(SpawnTimer, this, &AP9MonsterSpawner::SpawnEvent, SpawnFrequency, true);
	}

}

void AP9MonsterSpawner::SpawnEvent()
{
	if (GameState == nullptr) return;

	if (GameState->WaveSettings.IsValidIndex(CurrentWaveIndex))
	{
		const FP9WaveData& CurrentWaveData = GameState->WaveSettings[CurrentWaveIndex];

		for (const FSpawnData& SpawnData : CurrentWaveData.SpawnList)
		{
			if (SpawnData.MonsterClass != nullptr)
			{
				for (int32 i = 0; i < SpawnData.SpawnCount; ++i)
				{
					FVector SpawnLocation;
					if(FindSpawnLocation(SpawnLocation))
					{
						GetWorld()->SpawnActor<AP9Monster>(SpawnData.MonsterClass, SpawnLocation, FRotator::ZeroRotator);
					}

				}
			}
		}
	}
}

bool AP9MonsterSpawner::FindSpawnLocation(FVector& OutSpawnLocation)
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (PlayerCharacter == nullptr) return false;

	const FVector PlayerLocation = PlayerCharacter->GetActorLocation();

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSystem == nullptr) return false;

	for (int32 i = 0; i < 1000; ++i)
	{
		float RandomAngle = FMath::RandRange(0.0f, 360.0f);
		FVector RandomDirection = FRotator(0.0f, RandomAngle, 0.0f).Vector();
		float RandomDistance = FMath::RandRange(MinSpawnRadius, MaxSpawnRadius);

		FVector SpawnPlace = PlayerLocation + (RandomDirection * RandomDistance);

		FNavLocation ResultLocation;
		bool bFound = NavSystem->ProjectPointToNavigation(SpawnPlace, ResultLocation, FVector(500.0f));

		if (bFound)
		{
			OutSpawnLocation = ResultLocation.Location;
			return true;
		}
	}

	return false;
}