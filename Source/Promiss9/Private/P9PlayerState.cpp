#include "P9PlayerState.h"
#include "P9Character.h"
#include "Algo/RandomShuffle.h"
#include "P9WeaponData.h"


AP9PlayerState::AP9PlayerState()
{

	CurrentLevel = 1;

	CurrentXP = 0.0f;

	CurrentGold = 0;

	XPForNextLevel = 50;

	Killcount = 0;

	BonusHeadshotChance = 0.0f;
	BonusHeadshotDamage = 0.0f;
	BonusDamagePer = 0.0f;
	BonusReloadSpeed = 0.0f;
	BonusLuck = 0.0f;
}

void AP9PlayerState::GetRewardDetail(EP9Stat Stat, EP9Rarity Rarity, float& OutValue, FString& OutDescription)
{
	if (RewardStatTable == nullptr) return;
	FName RowName;

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



void AP9PlayerState::AddXP(float XPAmount)
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
		FP9LevelUpReward Choice;
		float Value;
		FString Desc;
		Choice.Stat = (SelectedStat);
		Choice.Rarity = (SelectedRarity);
		GetRewardDetail(SelectedStat, SelectedRarity, Value, Desc);
		Choice.Description = (Desc);

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
			Player->AddMaxHealth(ValueToApply);
			Player->AddHealth(ValueToApply);
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

void AP9PlayerState::AddKillCount()
{
	Killcount++;
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

//상점 연동용

int32 AP9PlayerState::GetGold() const
{
	return CurrentGold;
}

bool AP9PlayerState::CanAfford(int32 Cost) const
{
	// 0원 이하라면 항상 가능, 그 외엔 보유 골드로 판단
	return (Cost <= 0) || (CurrentGold >= Cost);
}

bool AP9PlayerState::SpendGold(int32 Cost)
{
	if (Cost <= 0) return true;
	if (CurrentGold < Cost) return false;

	CurrentGold -= Cost;
	return true;
}

void AP9PlayerState::AddWeaponDamageBonus(FName WeaponId, int32 FlatBonus)
{
	if (WeaponId.IsNone() || FlatBonus == 0) return; //무기 ID가 없으면 무시하는 거

	WeaponFlatBonus.FindOrAdd(WeaponId) += FlatBonus; // 이미 있는 무기가 추가되면 값을 누적, 없으면 새로 추가하기
}

bool AP9PlayerState::GetEffectiveDamage(FName WeaponId, float& OutDamage) const
{
	OutDamage = 0.0f;
	if (!WeaponDataTable || WeaponId.IsNone()) return false;

	const FP9WeaponData* Row = WeaponDataTable->FindRow<FP9WeaponData>(
		WeaponId, TEXT("PS_GetEffectiveDamage"), false); //DataTable에서 무기의 기본값을 찾기, row를 못찾으면 찾을때 쓰는용 문구.
	if (!Row) return false;

	const int32* Flat = WeaponFlatBonus.Find(WeaponId);
	const int32 Add = Flat ? *Flat : 0;

	OutDamage = Row->Damage + Add;
	return true;
}

//Damage
float AP9PlayerState::DamageCalculation(float WeaponDamage, bool& bHeadshot)
{
	bHeadshot = IsHeadshot();
	if (bHeadshot)
	{
		return WeaponDamage * (1 + (BonusDamagePer / 100)) * (1.5 + (BonusHeadshotDamage / 100));
	}

	return WeaponDamage * (1 + (BonusDamagePer / 100));
}

bool AP9PlayerState::IsHeadshot()
{
	if(BonusHeadshotChance<=0.0f) return false;

	float RandomRoll = FMath::FRandRange(0.0f, 100.0f);

	if (RandomRoll < BonusHeadshotChance) return true;

	return false;
}
