#include "P9PlayerState.h"
#include "Algo/RandomShuffle.h"
AP9PlayerState::AP9PlayerState()
{
	
	CurrentLevel = 1;
	
	CurrentXP=0;
	
	XPForNextLevel=50;

	BonusHeadshotChance=0.0f;
	BonusHeadshotDamage=0.0f;
	BonusDamagePer=0.0f;
	BonusReloadSpeed=0.0f;
	BonusLuck=0.0f;
}

void AP9PlayerState::GetRewardDetail(EP9Stat Stat, EP9Rarity Rarity, float& OutValue, FString& OutDescription)
{
	OutValue = 0.0f;
	OutDescription = TEXT("EMPTYDATA");

}



void AP9PlayerState::AddXP(int32 XPAmount)
{
	if (CurrentXP < XPForNextLevel)
	{
		CurrentXP += XPAmount;
	}
	if (CurrentXP >= XPForNextLevel)
	{
		LevelUp();
	}
}



void AP9PlayerState::LevelUp()
{
	CurrentXP -= XPForNextLevel;
	CurrentLevel += 1;
	XPForNextLevel += 15;
	TArray<FP9LevelUpReward> Choices = GenerateReward();
	LevelUpUI(Choices);
}
TArray<FP9LevelUpReward> AP9PlayerState::GenerateReward()
{
	TArray<EP9Stat> StatPool;
	for (int32 i = 0; i < (int32)EP9Stat::MAX; ++i)
	{
		StatPool.Add(EP9Stat(i));
	}
	Algo::RandomShuffle(StatPool);

	TArray<FP9LevelUpReward> Choices;

	for (int32 i = 0; i < 3; ++i)
	{
		EP9Stat SelectedStat = StatPool[i];
		EP9Rarity SelectedRarity = (EP9Rarity)FMath::RandRange(0, (int32)EP9Rarity::MAX - 1);
		FString SelectedDescription = TEXT("X등급 능력치 이름 X증가");
		FP9LevelUpReward Choice;
		Choice.Stat=(SelectedStat);
		Choice.Rarity=(SelectedRarity);
		Choice.Description=(SelectedDescription);

		Choices.Add(Choice);
	}

	return Choices;
}
void AP9PlayerState::ApplyReward(const FP9LevelUpReward& Selected)
{

}


