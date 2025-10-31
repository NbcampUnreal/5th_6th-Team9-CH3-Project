#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Engine/DataTable.h"
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
struct FP9LevelUpReward
{
	GENERATED_BODY()

	FP9LevelUpReward()
	{
		Stat = EP9Stat::MAX;
		Rarity = EP9Rarity::MAX;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reward")
	EP9Stat Stat;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reward")
	EP9Rarity Rarity;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reward")
	FString Description;
};

USTRUCT(BlueprintType)
struct FP9RewardStatData : public FTableRowBase
{
	GENERATED_BODY()

	FP9RewardStatData()
	{
		Common = 0.0f;
		Uncommon = 0.0f;
		Rare = 0.0f;
		Legendary = 0.0f;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString StatName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Common;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Uncommon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Rare;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Legendary;
};

UCLASS()
class PROMISS9_API AP9PlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	AP9PlayerState();
	UFUNCTION(BlueprintCallable)
	void AddXP(float XPAmount);
	UFUNCTION(BlueprintCallable)
	void AddGold(int32 GoldAmount);

	UFUNCTION(BlueprintCallable)
	void AddKillCount();

	float GetBonusHeadshotDamage() const;
	float GetBonusHeadshotChance() const;
	float GetBonusDamagePer() const;
	float GetBonusReloadSpeed() const;
	float GetBonusLuck() const;


	// 상점 연동용 골드 계산
	UFUNCTION(BlueprintPure, Category = "Gold")
	int32 GetGold() const;

	UFUNCTION(BlueprintPure, Category = "Gold")
	bool CanAfford(int32 Cost) const;

	UFUNCTION(BlueprintCallable, Category = "Gold")
	bool SpendGold(int32 Cost);


private:
	void GetRewardDetail(EP9Stat Stat, EP9Rarity Rarity, float& OutValue, FString& OutDescription);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevel;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	float CurrentXP;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 XPForNextLevel;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gold")
	int32 CurrentGold;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "KillCount")
	int32 Killcount;



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

	UPROPERTY(EditDefaultsOnly, Category = " Level")
	TObjectPtr<UDataTable> RewardStatTable=nullptr;

	
	void LevelUp();
	TArray<FP9LevelUpReward> GenerateReward();
	UFUNCTION(BlueprintCallable)
	void ApplyReward(const FP9LevelUpReward& Selected);
	UFUNCTION(BlueprintImplementableEvent, Category = "Level")
	void LevelUpUI(const TArray<FP9LevelUpReward>& Reward);
};
