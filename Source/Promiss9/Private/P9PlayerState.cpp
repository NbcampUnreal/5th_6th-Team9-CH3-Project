#include "P9PlayerState.h"
#include "P9Character.h"
#include "Algo/RandomShuffle.h"

#include "P9WeaponData.h"

AP9PlayerState::AP9PlayerState()
{
	CurrentLevel = 1;
	
	CurrentXP = 0.f;

	CurrentGold = 0;

	XPForNextLevel=50;

	Killcount = 0;

	BonusHeadshotChance=0.0f;
	BonusHeadshotDamage=0.0f;
	BonusDamagePer=0.0f;
	BonusReloadSpeed=0.0f;
	BonusLuck=0.0f;
}

void AP9PlayerState::GetRewardDetail(EP9Stat Stat, EP9Rarity Rarity, float& OutValue, FString& OutDescription)
{
	if (!RewardStatTable) return;

	FName RowName = NAME_None;
	switch (Stat)
	{
	case EP9Stat::MoveSpeed:       RowName = TEXT("MoveSpeed");      break;
	case EP9Stat::Health:          RowName = TEXT("Health");         break;
	case EP9Stat::HeadshotChance:  RowName = TEXT("HeadshotChance"); break;
	case EP9Stat::HeadshotDamage:  RowName = TEXT("HeadshotDamage"); break;
	case EP9Stat::ReloadSpeed:     RowName = TEXT("ReloadSpeed");    break;
	case EP9Stat::DamagePer:       RowName = TEXT("DamagePer");      break;
	case EP9Stat::Luck:            RowName = TEXT("Luck");           break;
	default: return;
	}

	const FP9RewardStatData* Row = RewardStatTable->FindRow<FP9RewardStatData>(RowName, TEXT("P9PS_GetRewardDetail"), false);
	if (!Row) return;

	switch (Rarity)
	{
	case EP9Rarity::Common:    OutValue = Row->Common;    break;
	case EP9Rarity::Uncommon:  OutValue = Row->Uncommon;  break;
	case EP9Rarity::Rare:      OutValue = Row->Rare;      break;
	case EP9Rarity::Legendary: OutValue = Row->Legendary; break;
	default: return;
	}

	const TCHAR* RarityText = TEXT("");
	switch (Rarity)
	{
	case EP9Rarity::Common:    RarityText = TEXT("일반");  break;
	case EP9Rarity::Uncommon:  RarityText = TEXT("고급");  break;
	case EP9Rarity::Rare:      RarityText = TEXT("희귀");  break;
	case EP9Rarity::Legendary: RarityText = TEXT("전설");  break;
	default: break;
	}

	OutDescription = FString::Printf(TEXT("[%s] %s : +%.0f%% 증가"), RarityText, *Row->StatName, OutValue);
}

void AP9PlayerState::AddXP(float XPAmount)
{
	if (XPAmount <= 0.f) return;

	CurrentXP += XPAmount;
	while (CurrentXP >= XPForNextLevel)
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
	TArray<EP9Stat> Pool;
	for (int32 i = 0; i < static_cast<int32>(EP9Stat::MAX); ++i)
	{
		Pool.Add(static_cast<EP9Stat>(i));
	}
	Algo::RandomShuffle(Pool);

	TArray<FP9LevelUpReward> Out;
	for (int32 i = 0; i < 3 && i < Pool.Num(); ++i)
	{
		EP9Stat   S = Pool[i];
		EP9Rarity R = static_cast<EP9Rarity>(FMath::RandRange(0, static_cast<int32>(EP9Rarity::MAX) - 1));

		float   Val = 0.f;
		FString Desc;
		GetRewardDetail(S, R, Val, Desc);

		FP9LevelUpReward Rw;
		Rw.Stat = S;
		Rw.Rarity = R;
		Rw.Description = Desc;
		Out.Add(Rw);
	}
	return Out;
}

void AP9PlayerState::ApplyReward(const FP9LevelUpReward& Selected)
{
	float   Val = 0.f;
	FString Ignored;
	GetRewardDetail(Selected.Stat, Selected.Rarity, Val, Ignored);

	switch (Selected.Stat)
	{
	case EP9Stat::MoveSpeed:
	{
		if (AP9Character* P = Cast<AP9Character>(GetPawn()))
		{
			P->AddNormalSpeed(Val);
		}
		break;
	}
	case EP9Stat::Health:
	{
		if (AP9Character* P = Cast<AP9Character>(GetPawn()))
		{
			P->AddMaxHealth(Val);
			P->AddHealth(Val);
		}
		break;
	}
	case EP9Stat::HeadshotChance:  BonusHeadshotChance += Val; break;
	case EP9Stat::HeadshotDamage:  BonusHeadshotDamage += Val; break;
	case EP9Stat::DamagePer:       BonusDamagePer += Val; break;
	case EP9Stat::ReloadSpeed:     BonusReloadSpeed += Val; break;
	case EP9Stat::Luck:            BonusLuck += Val; break;
	default: break;
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

int32 AP9PlayerState::GetGold() const
{
	return CurrentGold;
}

bool AP9PlayerState::CanAfford(int32 Cost) const
{
	return (Cost <= 0) || (CurrentGold >= Cost);
}

bool AP9PlayerState::SpendGold(int32 Cost)
{
	if (Cost <= 0) return true;
	if (CurrentGold < Cost) return false;
	CurrentGold -= Cost;
	return true;
}

float AP9PlayerState::GetBonusHeadshotDamage() const { return BonusHeadshotDamage; }
float AP9PlayerState::GetBonusHeadshotChance() const { return BonusHeadshotChance; }
float AP9PlayerState::GetBonusDamagePer()     const { return BonusDamagePer; }
float AP9PlayerState::GetBonusReloadSpeed()   const { return BonusReloadSpeed; }
float AP9PlayerState::GetBonusLuck()          const { return BonusLuck; }

//상점 연동
void AP9PlayerState::AddWeaponDamageBonus(FName WeaponId, int32 FlatBonus)
{
	if (WeaponId.IsNone() || FlatBonus == 0) return;
	WeaponFlatBonus.FindOrAdd(WeaponId) += FlatBonus;
}

bool AP9PlayerState::GetEffectiveDamage(FName WeaponId, float& OutDamage) const
{
	OutDamage = 0.f;
	if (!WeaponDataTable || WeaponId.IsNone()) return false;

	// 기본 Damage 조회
	const FP9WeaponData* Row = WeaponDataTable->FindRow<FP9WeaponData>(WeaponId, TEXT("PS_GetEffectiveDamage"), false);
	if (!Row) return false;

	const int32* Flat = WeaponFlatBonus.Find(WeaponId);
	const int32  Add = Flat ? *Flat : 0;

	OutDamage = Row->Damage + Add;
	return true;
}
