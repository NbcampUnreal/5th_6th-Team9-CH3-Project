#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "P9PlayerState.generated.h"


UENUM(BlueprintType)
enum class EP9Stat : uint8
{
	MoveSpeed,
	Health,
	HeadshotChance,
	HeadshotDamage,
	DamagePer,
	ReloadSpeed,
	Luck,
	
	MAX
};

UENUM(BlueprintType)
enum class EP9Rarity : uint8
{
	Common,
	Uncommon,
	Rare,
	Legendary,

	MAX
};

USTRUCT(BlueprintType)
struct P9LevelUpReward
{
	GENERATED_BODY()

	EP9Stat Stat;
	EP9Rarity Rarity;

	FString 
};

UCLASS()
class PROMISS9_API AP9PlayerState : public APlayerState
{
	GENERATED_BODY()
	
	AP9PlayerState();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevel;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentXP;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 XPForNextLevel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BonusStats")
	float BonusHeadshotChance;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BonusStats")
	float BonusHeadshotDamage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BonusStats")
	float BonusDamagePer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BonusStats")
	float BonusReloadSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BonusStats")
	float BonusLuck;
};
