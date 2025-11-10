#include "P9SlimeMonster.h"

AP9SlimeMonster::AP9SlimeMonster()
{
    // 슬라임 기본 스탯
    HP = 50.f;             // 체력
    AttackPower = 8.f;     // 공격력
    ExpReward = 5;         // 경험치 드랍량
    GoldReward = 20;        // 골드 드랍량
}

void AP9SlimeMonster::BeginPlay()
{
    Super::BeginPlay();

    // 추가 초기화가 필요하면 여기서 처리 가능
    // 예: 슬라임 전용 파티클, 사운드 등
}