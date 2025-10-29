#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "P9Shop.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class USphereComponent;

UCLASS()
class PROMISS9_API AP9Shop : public AActor
{
    GENERATED_BODY()

public:
    AP9Shop();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop|Comp")
    UCapsuleComponent* Capsule;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop|Comp")
    USkeletalMeshComponent* Mesh;

    // 상호작용 범위용
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop|Comp")
    USphereComponent* TalkSphere;

    // 상점의 표시 이름
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
    FText ShopName = FText::FromString(TEXT("Wandering Merchant"));

protected:
    virtual void BeginPlay() override;

    // 플레이어가 범위에 들어왔/나갔을 때
    UFUNCTION()
    void OnTalkBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Sweep);

    UFUNCTION()
    void OnTalkEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
    UFUNCTION(BlueprintCallable, Category = "Shop")
    void Interact(AActor* Interactor);

    UFUNCTION(BlueprintImplementableEvent, Category = "Shop")
    void OpenShop(AActor* Interactor);

    // 범위 안에 플레이어가 있는지 간단 체크
    UFUNCTION(BlueprintPure, Category = "Shop")
    bool IsPlayerInRange() const { return bPlayerInRange; }

private:
    bool bPlayerInRange = false;
    TWeakObjectPtr<AActor> CachedInteractor; 
};
