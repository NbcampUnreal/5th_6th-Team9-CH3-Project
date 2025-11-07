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

	bLevelUp = false;

	BonusHeadshotChance = 0.0f;
	BonusHeadshotDamage = 0.0f;
	BonusDamagePer = 0.0f;
	BonusHealthRegen = 0.0f;
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
	case EP9Stat::HealthRegen:
	{
		RowName = FName("HealthRegen");
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

	OutDescription = FString::Printf(TEXT("%s : +%.2f%% 증가"), *RowData->StatName, OutValue);
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
	if (bLevelUp == false)
	{
		bLevelUp = true;
		LevelUpUI(Choices);
	}
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
		EP9Rarity SelectedRarity;



		float LuckCommon = BonusLuck * (-0.2f);
		float LuckUncommon = BonusLuck * (-0.1f);
		float LuckRare = BonusLuck * (0.2f);
		float LuckLegendary = BonusLuck * (0.1f);

		float FinalProbCommon = FMath::Max(0.0f, ProbCommon + LuckCommon);
		float FinalProbUncommon = FMath::Max(0.0f, ProbUncommon + LuckUncommon);
		float FinalProbRare = FMath::Max(0.0f, ProbRare + LuckRare);
		float FinalProbLegendary = FMath::Max(0.0f, ProbLegendary + LuckLegendary);
		float TotalProb = FinalProbCommon + FinalProbUncommon + FinalProbRare + FinalProbLegendary;

		const float RandomRoll = FMath::FRandRange(0.0f, TotalProb);

		float CurrentPer = FinalProbCommon;
		if (RandomRoll < CurrentPer)
		{
			SelectedRarity = EP9Rarity::Common;
		}
		else
		{
			CurrentPer += FinalProbUncommon;
			if (RandomRoll < CurrentPer)
			{
				SelectedRarity = EP9Rarity::Uncommon;
			}
			else
			{
				CurrentPer += FinalProbRare;
				if (RandomRoll < CurrentPer)
				{
					SelectedRarity = EP9Rarity::Rare;
				}
				else
				{
					SelectedRarity = EP9Rarity::Legendary;
				}
			}
		}
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
	case EP9Stat::HealthRegen:
	{
		BonusHealthRegen += ValueToApply;
		break;
	}
	case EP9Stat::Luck:
	{
		BonusLuck += ValueToApply;
		break;
	}
	}
	OnLevelUP.Broadcast();
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

float AP9PlayerState::GetBonusHealthRegen() const
{
	return BonusHealthRegen;
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

#pragma region ShopWeapon
void AP9PlayerState::AddWeaponDamageBonus(FName WeaponId, float FlatBonus)
{
	if (WeaponId.IsNone() || FMath::IsNearlyZero(FlatBonus))
		return;

	float& Stored = WeaponFlatBonus.FindOrAdd(WeaponId);
	Stored += FlatBonus;
}

bool AP9PlayerState::GetEffectiveDamage(FName WeaponId, float& OutDamage) const
{
	OutDamage = 0.0f;
	if (!WeaponDataTable || WeaponId.IsNone()) return false;

	const FP9WeaponData* Row = WeaponDataTable->FindRow<FP9WeaponData>(
		WeaponId,
		TEXT("PS_GetEffectiveDamage"),
		false);
	if (!Row) return false;

	const float* Flat = WeaponFlatBonus.Find(WeaponId);
	const float Add = Flat ? *Flat : 0.f;

	OutDamage = Row->Damage + Add;
	return true;
}

void AP9PlayerState::AddWeaponRangeBonus(FName WeaponId, float BonusAmount)
{
	if (WeaponId.IsNone() || FMath::IsNearlyZero(BonusAmount)) return;

	float& Stored = WeaponRangeBonusMap.FindOrAdd(WeaponId);
	Stored += BonusAmount;
}
bool AP9PlayerState::GetEffectiveRange(FName WeaponId, float& OutRange) const
{
	if (!WeaponDataTable || WeaponId.IsNone()) return false;

	const FP9WeaponData* Row = WeaponDataTable->FindRow<FP9WeaponData>(
		WeaponId,
		TEXT("PS_GetEffectiveRange"));
	if (!Row) return false;

	const float* Bonus = WeaponRangeBonusMap.Find(WeaponId);
	const float Add = Bonus ? *Bonus : 0.f;

	OutRange = Row->Range + Add;
	return true;
}

void AP9PlayerState::AddWeaponFireSpeedBonus(FName WeaponId, float BonusAmount)
{
	if (WeaponId.IsNone() || FMath::IsNearlyZero(BonusAmount)) return;

	float& Stored = WeaponFireSpeedBonusMap.FindOrAdd(WeaponId);
	Stored += BonusAmount;
}

bool AP9PlayerState::GetEffectiveFireSpeed(FName WeaponId, float& OutSpeed) const
{
	if (!WeaponDataTable || WeaponId.IsNone()) return false;

	const FP9WeaponData* Row = WeaponDataTable->FindRow<FP9WeaponData>(
		WeaponId,
		TEXT("PS_GetEffectiveFireSpeed"));
	if (!Row) return false;

	const float* Bonus = WeaponFireSpeedBonusMap.Find(WeaponId);
	const float Add = Bonus ? *Bonus : 0.f;

	OutSpeed = Row->FireSpeed + Add;
	return true;
}
#pragma endregion

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
	if (BonusHeadshotChance <= 0.0f) return false;

	float RandomRoll = FMath::FRandRange(0.0f, 100.0f);

	if (RandomRoll < BonusHeadshotChance) return true;

	return false;
}
