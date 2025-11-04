#include "P9GameMode.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"

AP9GameMode::AP9GameMode()
{
	ShopRespawnTimer = ShopSpawnFrequency;
	CurrentShop = nullptr;
	TimeUntilPenaltyUp = PenaltyUpFrequency;
}

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

void AP9GameMode::SpawnShop()
{
	if (ShopClass == nullptr) return;

	FVector SpawnLocation;

	if (FindShopSpawnLocation(SpawnLocation))
	{
		CurrentShop = GetWorld()->SpawnActor<AP9Shop>(ShopClass, SpawnLocation, FRotator::ZeroRotator);
	}
}

bool AP9GameMode::FindShopSpawnLocation(FVector& OutLocation)
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (PlayerCharacter == nullptr) return false;

	const FVector PlayerLocation = PlayerCharacter->GetActorLocation();

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSystem == nullptr) return false;

	for (int32 i = 0; i < 10; ++i)
	{
		float RandomAngle = FMath::RandRange(0.0f, 360.0f);
		FVector RandomDirection = FRotator(0.0f, RandomAngle, 0.0f).Vector();
		float RandomDistance = FMath::RandRange(ShopMinSpawnRadius, ShopMaxSpawnRadius);

		FVector SpawnPlace = PlayerLocation + (RandomDirection * RandomDistance);

		FNavLocation ResultLocation;
		bool bFound = NavSystem->ProjectPointToNavigation(SpawnPlace, ResultLocation, FVector(500.0f));

		if (bFound)
		{
			OutLocation = ResultLocation.Location;
			return true;
		}
	}

	return false;
}

void AP9GameMode::UpdateTimer()
{
	//WaveSystem Timer
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
	//ShopSystem Timer
	if (CurrentShop == nullptr)
	{
		ShopRespawnTimer -= 1.0f;

		if (ShopRespawnTimer <= 0.0f)
		{
			SpawnShop();
			ShopRespawnTimer = ShopSpawnFrequency;
		}
	}

	//Penalty

	if (bIsPenaltyActive == false)
	{
		if (P9GameState->CurrentGameTime >= PenaltyStartTime)
		{
			bIsPenaltyActive = true;
		}
	}

	if (bIsPenaltyActive == true)
	{
		float DamageToApply = PenaltyDamage + (10 * (CurrentPenaltyLevel - 1));

		ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		AP9Character* P9Player = Cast<AP9Character>(Player);

		if (P9Player != nullptr)
		{
			P9Player->ApplyPenaltyDamage(DamageToApply);
		}

		TimeUntilPenaltyUp -= 1.0f;
		if (TimeUntilPenaltyUp <= 0.0f)
		{
			CurrentPenaltyLevel++;
			TimeUntilPenaltyUp = PenaltyUpFrequency;
		}
	}

}

void AP9GameMode::OnShopPurchased()
{
	CurrentShop = nullptr;
	ShopRespawnTimer = ShopSpawnFrequency;
}