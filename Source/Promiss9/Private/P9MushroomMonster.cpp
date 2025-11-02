// Fill out your copyright notice in the Description page of Project Settings.


#include "P9MushroomMonster.h"

AP9MushroomMonster::AP9MushroomMonster()
{
    // 버섯
    HP = 100.f;             // 체력
    AttackPower = 12.f;     // 공격력
    ExpReward = 7;         // 경험치 드랍량
    GoldReward = 4;        // 골드 드랍량
}

void AP9MushroomMonster::BeginPlay()
{
    Super::BeginPlay();

    // 추가 초기화가 필요하면 여기서 처리 가능
    // 예: 슬라임 전용 파티클, 사운드 등
}