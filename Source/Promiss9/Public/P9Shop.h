#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "P9Shop.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UDataTable;
class APawn;

UENUM(BlueprintType)
enum class EP9ShopRarity : uint8
{
	Common,
	Uncommon,
	Rare,
	Legendary,
	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EP9ShopStatType : uint8
{
	None,
	Damage,
	Range,
	FireSpeed
};

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

	//공격력 뽀너스

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
	EP9ShopStatType StatType = EP9ShopStatType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
	float DamageBonus = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
	float RangeBonus = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
	float FireSpeedBonus = 0.f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPressE);

UCLASS()
class PROMISS9_API AP9Shop : public AActor
{
	GENERATED_BODY()

public:
	AP9Shop();

	UPROPERTY(BlueprintAssignable, Category = "Key")
	FOnPressE OnPressE;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere, Category = "Shop")
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Shop")
	USphereComponent* Trigger;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Data", meta = (AllowPrivateAccess = "true"))
	UDataTable* WeaponDataTable = nullptr;

	// 가격 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Rule", meta = (AllowPrivateAccess = "true"))
	int32 Price_Common = 50;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Rule", meta = (AllowPrivateAccess = "true"))
	int32 Price_Uncommon = 100;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Rule", meta = (AllowPrivateAccess = "true"))
	int32 Price_Rare = 150;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Rule", meta = (AllowPrivateAccess = "true"))
	int32 Price_Legendary = 300;

	// 등급 확률
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Rule", meta = (ClampMin = "0.0", ClampMax = "1.0", AllowPrivateAccess = "true"))
	float Prob_Common = 0.60f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Rule", meta = (ClampMin = "0.0", ClampMax = "1.0", AllowPrivateAccess = "true"))
	float Prob_Uncommon = 0.25f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Rule", meta = (ClampMin = "0.0", ClampMax = "1.0", AllowPrivateAccess = "true"))
	float Prob_Rare = 0.10f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Rule", meta = (ClampMin = "0.0", ClampMax = "1.0", AllowPrivateAccess = "true"))
	float Prob_Legendary = 0.05f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop")
	TArray<FShopOffer> CurrentOffers;

	UPROPERTY(VisibleAnywhere, Category = "Shop")
	bool bPlayerInRange = false;

	UPROPERTY()
	APlayerController* CachedPC = nullptr;
	// 요기 추가됨
	UPROPERTY()
	APawn* OverlappedPawn = nullptr;

	bool bInteractBound = false;

	bool bOffersBuilt = false;

public:
	UFUNCTION(BlueprintCallable, Category = "Shop")
	void BuildOffers();

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void RerollOffers();

	UFUNCTION(BlueprintPure, Category = "Shop")
	const TArray<FShopOffer>& GetOffers() const { return CurrentOffers; }
	//이거 블루프린트
	UFUNCTION(BlueprintPure, Category = "Shop")
	FShopOffer GetOffer(int32 INdex) const;

	UFUNCTION(BlueprintCallable, Category = "Shop")
	bool TryPurchaseWithCurrentPawn(int32 OfferIndex);

	UFUNCTION(BlueprintCallable, Category = "Shop")
	bool TryPurchase(int32 OfferIndex, APawn* BuyerPawn);

	UFUNCTION(BlueprintCallable, Category = "Shop")
	bool TryReroll(APawn* BuyerPawn);

	UFUNCTION(BlueprintPure, Category = "Shop|Helper")
	static FString GetStatTypeString(EP9ShopStatType StatType);

protected:
	UFUNCTION()
	void OnTriggerBegin(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp,
		int32 BodyIndex, bool bFromSweep, const FHitResult& Hit);
	UFUNCTION()
	void OnTriggerEnd(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp,
		int32 BodyIndex);

	void BindInput();
	void UnbindInput();

	void HandleInteract();

private:

	EP9ShopRarity RollRarity() const;
	int32 GetPriceByRarity(EP9ShopRarity Rarity) const;
	bool PickThreeDistinctWeapons(TArray<FName>& OutWeaponIds) const;

	// 희귀도 → 데미지 보정 값(+3/+6/+9/+12)
	float GetDamageBonusByRarity(EP9ShopRarity Rarity) const;
	float GetRangeBonusByRarity(EP9ShopRarity Rarity) const;
	float GetFireSpeedBonusByRarity(EP9ShopRarity Rarity) const;
};
