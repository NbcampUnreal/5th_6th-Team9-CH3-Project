#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectPtr.h"
#include "P9GameState.h"
#include "P9Shop.h"
#include "GameFramework/GameModeBase.h"
#include "P9GameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROMISS9_API AP9GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AP9GameMode();
	void OnShopPurchased();

private:

	UPROPERTY()
	TObjectPtr<AP9GameState> P9GameState;
	FTimerHandle WaveTimer;

	UPROPERTY(EditDefaultsOnly, Category = "Shop")
	TSubclassOf<AP9Shop> ShopClass;
	UPROPERTY(EditDefaultsOnly, Category = "Shop")
	float ShopSpawnFrequency = 20.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Shop")
	float ShopMinSpawnRadius = 1000.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Shop")
	float ShopMaxSpawnRadius = 5000.0f;

	float ShopRespawnTimer;
	TObjectPtr<AP9Shop> CurrentShop;
	
	void SpawnShop();

	bool FindShopSpawnLocation(FVector& OutLocation);

	void UpdateTimer();
	void BeginPlay() override;
};
