#include "P9PlayerState.h"
#include "P9Character.h"
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

	switch (Stat)
	{
	case EP9Stat::MoveSpeed:
	{
		RowName = FName("MoveSpeed");
		break;
	}

	case EP9Stat::Health:
	{
		RowName = FName("Health");
		break;
	}
	case EP9Stat::HeadshotChance:
	{
		RowName = FName("HeadshotChance");
		break;
	}

	case EP9Stat::HeadshotDamage:
	{
		RowName = FName("HeadshotDamage");
		break;
	}
	case EP9Stat::ReloadSpeed:
	{
		RowName = FName("ReloadSpeed");
		break;
	}
	case EP9Stat::DamagePer:
	{
		RowName = FName("DamagePer");
		break;
	}

	case EP9Stat::Luck:
	{
		RowName = FName("Luck");
		break;
	}
	}

	FP9RewardStatData* RowData = RewardStatTable->FindRow<FP9RewardStatData>(RowName, TEXT(""));
	if (RowData == nullptr) return;
	
	switch (Rarity)
	{
	case EP9Rarity::Common:
	{
		OutValue = RowData->Common;
		break;
	}
	case EP9Rarity::Uncommon:
	{
		OutValue = RowData->Uncommon;
		break;
	}
	case EP9Rarity::Rare:
	{
		OutValue = RowData->Rare;
		break;
	}
	case EP9Rarity::Legendary:
	{
		OutValue = RowData->Legendary;
		break;
	}
	}

	FString RarityString;
	switch (Rarity)
	{
	case EP9Rarity::Common:
	{
		RarityString = TEXT("일반");
		break;
	}
	case EP9Rarity::Uncommon:
	{
		RarityString = TEXT("고급");
		break;
	}
	case EP9Rarity::Rare:
	{
		RarityString = TEXT("희귀");
		break;
	}
	case EP9Rarity::Legendary:
	{
		RarityString = TEXT("전설");
		break;
	}
	}

	OutDescription = FString::Printf(TEXT("[%s] %s : +%.0f%% 증가"), *RarityString, *RowData->StatName, OutValue);
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
	float ValueToApply;
	FString IgnoredDesc;
	GetRewardDetail(Selected.Stat, Selected.Rarity, ValueToApply, IgnoredDesc);
	switch (Selected.Stat)
	{
	case EP9Stat::MoveSpeed:
	{
		AP9Character* Player = Cast<AP9Character>(GetPawn());
		if (Player != nullptr)
		{
			Player->AddNormalSpeed(ValueToApply);
		}
		break;
	}
	case EP9Stat::Health:
	{
		AP9Character* Player = Cast<AP9Character>(GetPawn());
		if (Player != nullptr)
		{
			
		}
		break;
	}
	case EP9Stat::HeadshotChance:
	{
		BonusHeadshotChance += ValueToApply;
		break;
	}
	case EP9Stat::HeadshotDamage:
	{
		BonusHeadshotDamage += ValueToApply;
		break;
	}
	case EP9Stat::DamagePer:
	{
		BonusDamagePer += ValueToApply;
		break;
	}
	case EP9Stat::ReloadSpeed:
	{
		BonusReloadSpeed += ValueToApply;
		break;
	}
	case EP9Stat::Luck:
	{
		BonusLuck += ValueToApply;
		break;
	}
	}

}

void AP9PlayerState::AddGold(int32 GoldAmount)
{
	CurrentGold += GoldAmount;
}

float AP9PlayerState::GetBonusHeadshotDamage() const
{
	return BonusHeadshotDamage;
}

float AP9PlayerState::GetBonusHeadshotChance() const
{
	return BonusHeadshotChance;
}

float AP9PlayerState::GetBonusDamagePer() const
{
	return BonusDamagePer;
}

float AP9PlayerState::GetBonusReloadSpeed() const
{
	return BonusReloadSpeed;
}

float AP9PlayerState::GetBonusLuck() const
{
	return BonusLuck;
}


