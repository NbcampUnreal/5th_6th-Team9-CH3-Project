// Fill out your copyright notice in the Description page of Project Settings.


#include "P9DragonMonster.h"

AP9DragonMonster::AP9DragonMonster()
{
	// 드래곤 기본 스탯
    HP = 2000.f;             // 체력
    AttackPower = 20.f;     // 공격력
    ExpReward = 1;         // 경험치 드랍량
    GoldReward = 1;        // 골드 드랍량
}

void AP9DragonMonster::BeginPlay()
{
    Super::BeginPlay();

    // 추가 초기화가 필요하면 여기서 처리 가능
    // 예: 슬라임 전용 파티클, 사운드 등
}