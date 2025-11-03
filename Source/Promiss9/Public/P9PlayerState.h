#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Engine/DataTable.h"
#include "P9PlayerState.generated.h"


struct FP9WeaponData;
class UDataTable;

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
	EP9Stat   Stat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reward")
	EP9Rarity Rarity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reward")
	FString   Description;
};

// DataTable Row
USTRUCT(BlueprintType)
struct FP9RewardStatData : public FTableRowBase
{
	GENERATED_BODY()

	FP9RewardStatData()
		: Common(0.f), Uncommon(0.f), Rare(0.f), Legendary(0.f)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly) FString StatName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float  Common;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float  Uncommon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float  Rare;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float  Legendary;
};

UCLASS()
class PROMISS9_API AP9PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AP9PlayerState();

	// XP/레벨
	UFUNCTION(BlueprintCallable) void  AddXP(float XPAmount);

	// 골드
	UFUNCTION(BlueprintCallable, Category = "Gold") void   AddGold(int32 GoldAmount);
	UFUNCTION(BlueprintPure, Category = "Gold") int32  GetGold() const;
	UFUNCTION(BlueprintPure, Category = "Gold") bool   CanAfford(int32 Cost) const;
	UFUNCTION(BlueprintCallable, Category = "Gold") bool  SpendGold(int32 Cost);

	// 보너스 조회
	UFUNCTION(BlueprintPure) float GetBonusHeadshotDamage() const;
	UFUNCTION(BlueprintPure) float GetBonusHeadshotChance() const;
	UFUNCTION(BlueprintPure) float GetBonusDamagePer() const;
	UFUNCTION(BlueprintPure) float GetBonusReloadSpeed() const;
	UFUNCTION(BlueprintPure) float GetBonusLuck() const;

	//  상점 연동
	//Common/Uncommon/Rare/Legendary 구매 시 +3/+6/+9/+12
	UFUNCTION(BlueprintCallable, Category = "Weapons")
	void AddWeaponDamageBonus(FName WeaponId, int32 FlatBonus);

	UFUNCTION(BlueprintPure, Category = "Weapons")
	bool GetEffectiveDamage(FName WeaponId, float& OutDamage) const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level") int32 CurrentLevel;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level") float CurrentXP;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level") int32 XPForNextLevel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gold")  int32 CurrentGold;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BonusStats") float BonusHeadshotChance;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BonusStats") float BonusHeadshotDamage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BonusStats") float BonusDamagePer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BonusStats") float BonusReloadSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BonusStats") float BonusLuck;

	UPROPERTY(EditDefaultsOnly, Category = "Level")
	TObjectPtr<UDataTable> RewardStatTable = nullptr;

	// 무기 기본 수치 테이블
	UPROPERTY(EditDefaultsOnly, Category = "Weapons|Data")
	TObjectPtr<UDataTable> WeaponDataTable = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Weapons")
	TMap<FName, int32> WeaponFlatBonus;

	void LevelUp();
	TArray<FP9LevelUpReward> GenerateReward();
	UFUNCTION(BlueprintCallable) void ApplyReward(const FP9LevelUpReward& Selected);
	UFUNCTION(BlueprintImplementableEvent, Category = "Level") void LevelUpUI(const TArray<FP9LevelUpReward>& Reward);

private:
	void GetRewardDetail(EP9Stat Stat, EP9Rarity Rarity, float& OutValue, FString& OutDescription);
};
