#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "P9Monster.generated.h"

UCLASS()
class PROMISS9_API AP9Monster : public ACharacter
{
    GENERATED_BODY()

public:
    AP9Monster();

protected:
    virtual void BeginPlay() override;

    // 체력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Stats")
    float HP;

    // 공격력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Stats")
    float AttackPower;

    // 경험치 드랍량
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Stats")
    int32 ExpReward;

    // 골드 드랍량
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Stats")
    int32 GoldReward;

    // 데미지 간격 (초당 1회 등)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float DamageInterval;

    // 공격 딜레이용 타이머
    FTimerHandle DamageTimerHandle;

    // 현재 플레이어가 범위 내에 있는지
    bool bIsPlayerInRange;

    // 플레이어 참조
    UPROPERTY()
    AActor* TargetPlayer;

public:
    virtual void Tick(float DeltaTime) override;

    // 몬스터가 데미지를 입는 함수
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void TakeDamageFromPlayer(float DamageAmount);

    // 공격 범위에 플레이어가 들어왔을 때
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void StartDamagePlayer(AActor* PlayerActor);

    // 공격 범위에서 나갔을 때
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void StopDamagePlayer();

private:
    // 플레이어에게 일정 주기로 데미지를 주는 내부 함수
    void DealDamageToPlayer();
};