#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "P9Monster.generated.h"

UENUM(BlueprintType)
enum class EMonsterState : uint8
{
    Idle        UMETA(DisplayName = "Idle"),
    Chase       UMETA(DisplayName = "Chase"),
    Attack      UMETA(DisplayName = "Attack"),
    Dead        UMETA(DisplayName = "Dead")
};

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
    UMaterialInstanceDynamic* HitFlashMatInstance;

    // 체력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Stats")
    float HP;

    // 공격력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Stats")
    float AttackPower;

    // 경험치 드랍량
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Stats")
    float ExpReward;

    // 골드 드랍량
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster Stats")
    int32 GoldReward;


	// 사망 애니메이션 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    UAnimMontage* DeathAnimMontage;


	// Dissolve 효과 관련 변수

    float CurrentDissolveValue = 0.0f;

    // Dissolve 활성화 여부
    bool bIsDissolving = false;

    // Dissolve 총 시간 (초)
    float DissolveDuration = 2.0f;




public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster")
    EMonsterState CurrentState;

    // 몬스터가 데미지를 입는 함수
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void TakeDamageFromPlayer(float DamageAmount);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void PlayHitFlashEffect();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void ApplyKnockback();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Die();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void StartDissolveEffect();



};