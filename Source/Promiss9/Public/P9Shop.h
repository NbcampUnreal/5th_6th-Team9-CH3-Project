// P9Shop.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "P9Shop.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UDataTable;
class APawn;

/** 상점 전용 희귀도 */
UENUM(BlueprintType)
enum class EP9ShopRarity : uint8
{
	Common,
	Uncommon,
	Rare,
	Legendary,
	MAX UMETA(Hidden)
};

/** 상점 오퍼 데이터 */
USTRUCT(BlueprintType)
struct FShopOffer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
	FName WeaponId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
	EP9ShopRarity Rarity = EP9ShopRarity::Common;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
	int32 Price = 0;
};

UCLASS()
class PROMISS9_API AP9Shop : public AActor
{
	GENERATED_BODY()

public:
	AP9Shop();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** 간단 표시용 */
	UPROPERTY(VisibleAnywhere, Category = "Shop")
	UStaticMeshComponent* Mesh;

	/** 상호작용 범위 */
	UPROPERTY(VisibleAnywhere, Category = "Shop")
	USphereComponent* Trigger;

	/** 무기 데이터테이블(RowStruct = FP9WeaponData) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Data", meta = (AllowPrivateAccess = "true"))
	UDataTable* WeaponDataTable = nullptr;

	/** 희귀도별 가격(에디터 디테일 창 price에서 조절 가능) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Rule", meta = (AllowPrivateAccess = "true"))
	int32 Price_Common = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Rule", meta = (AllowPrivateAccess = "true"))
	int32 Price_Uncommon = 200;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Rule", meta = (AllowPrivateAccess = "true"))
	int32 Price_Rare = 400;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Rule", meta = (AllowPrivateAccess = "true"))
	int32 Price_Legendary = 800;

	/** 희귀도 출현 확률 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Rule", meta = (ClampMin = "0.0", ClampMax = "1.0", AllowPrivateAccess = "true"))
	float Prob_Common = 0.60f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Rule", meta = (ClampMin = "0.0", ClampMax = "1.0", AllowPrivateAccess = "true"))
	float Prob_Uncommon = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Rule", meta = (ClampMin = "0.0", ClampMax = "1.0", AllowPrivateAccess = "true"))
	float Prob_Rare = 0.10f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Rule", meta = (ClampMin = "0.0", ClampMax = "1.0", AllowPrivateAccess = "true"))
	float Prob_Legendary = 0.05f;

	/** 현재 오퍼 3칸 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop")
	TArray<FShopOffer> CurrentOffers;

	/** 플레이어 범위 체크 */
	UPROPERTY(VisibleAnywhere, Category = "Shop")
	bool bPlayerInRange = false;

	UPROPERTY()
	APlayerController* CachedPC = nullptr;

	bool bInteractBound = false;

public:
	/** 오퍼 3개 생성 */
	UFUNCTION(BlueprintCallable, Category = "Shop")
	void BuildOffers();

	/** 현재 오퍼 조회 */
	UFUNCTION(BlueprintPure, Category = "Shop")
	const TArray<FShopOffer>& GetOffers() const { return CurrentOffers; }

	/** 구매 시도 */
	UFUNCTION(BlueprintCallable, Category = "Shop")
	bool TryPurchase(int32 OfferIndex, APawn* BuyerPawn);

protected:
	/** 트리거 오버랩 */
	UFUNCTION()
	void OnTriggerBegin(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp,
		int32 BodyIndex, bool bFromSweep, const FHitResult& Hit);
	UFUNCTION()
	void OnTriggerEnd(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp,
		int32 BodyIndex);

	/** 입력 바인딩 */
	void BindInput();
	void UnbindInput();

	/** E: 오퍼 생성 */
	void HandleInteract();


private:
	/** 내부 유틸 */
	EP9ShopRarity RollRarity() const;
	int32 GetPriceByRarity(EP9ShopRarity Rarity) const;
	bool PickThreeDistinctWeapons(TArray<FName>& OutWeaponIds) const;
};
