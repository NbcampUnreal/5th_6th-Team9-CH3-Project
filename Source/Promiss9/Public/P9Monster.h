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

    //몬스터 메시
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster")
    USkeletalMeshComponent* MonsterMesh;

    //피격 이펙트용 머티리얼
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Effect")
    UMaterialInterface* HitFlashMaterial;

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


    //스피어 콜리전
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
    class USphereComponent* AttackRangeSphere;

    //공격 범위
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackRangeRadius;

    //오버렙 
    UFUNCTION()
    void OnAttackRangeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnAttackRangeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


public:
    virtual void Tick(float DeltaTime) override;

    // 몬스터가 데미지를 입는 함수
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void TakeDamageFromPlayer(float DamageAmount);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void PlayHitFlashEffect();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void ApplyKnockback();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void StartDamagePlayer(AActor* PlayerActor);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void StopDamagePlayer();


private:
    // 플레이어에게 일정 주기로 데미지를 주는 내부 함수
    void DealDamageToPlayer();
};