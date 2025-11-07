#include "P9Shop.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

#include "P9PlayerState.h"          
#include "P9InventoryComponent.h"  
#include "P9WeaponData.h"  
#include "P9GameMode.h"

AP9Shop::AP9Shop()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
	Trigger->SetupAttachment(RootComponent);
	Trigger->InitSphereRadius(200.f);
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Trigger->SetGenerateOverlapEvents(true);
}

void AP9Shop::BeginPlay()
{
	Super::BeginPlay();

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AP9Shop::OnTriggerBegin);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &AP9Shop::OnTriggerEnd);

	CachedPC = UGameplayStatics::GetPlayerController(this, 0);
}

void AP9Shop::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindInput();
	Super::EndPlay(EndPlayReason);
}


// 오퍼 3개(무기 중복 X) , 등급 랜덤

void AP9Shop::BuildOffers()
{
	if (bOffersBuilt)
		return;

	CurrentOffers.Reset();

	//   DataTable에 있는 전체 무기
	TArray<FName> CandidatePool;
	if (WeaponDataTable)
	{
		CandidatePool = WeaponDataTable->GetRowNames();
	}

	//  케릭터의 인벤토리가 꽉 차면 보유한 무기만 뜨게 함.
	if (OverlappedPawn)
	{
		if (UP9InventoryComponent* Inv = OverlappedPawn->FindComponentByClass<UP9InventoryComponent>())
		{
			TArray<FName> OwnedIds;
			Inv->GetCurrentWeaponIds(OwnedIds);  

			//  4개 이상이면 꽉 찼다고 본다
			const bool bInventoryFull = (OwnedIds.Num() >= 4);

			if (bInventoryFull && OwnedIds.Num() > 0)
			{
				CandidatePool = OwnedIds;
			}
		}
	}

	if (CandidatePool.Num() <= 0)
	{
#if !(UE_BUILD_SHIPPING)
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("[Shop] 후보 무기가 없습니다."));
#endif
		return;
	}

	for (int32 i = 0; i < 16; ++i)
	{
		const int32 A = FMath::RandRange(0, CandidatePool.Num() - 1);
		const int32 B = FMath::RandRange(0, CandidatePool.Num() - 1);
		if (A != B) CandidatePool.Swap(A, B);
	}

	const int32 OfferCount = FMath::Min(3, CandidatePool.Num());
	for (int32 i = 0; i < OfferCount; ++i)
	{
		FShopOffer Offer;
		Offer.WeaponId = CandidatePool[i];
		Offer.Rarity = RollRarity();
		Offer.Price = GetPriceByRarity(Offer.Rarity);

		const int32 OptionType = FMath::RandRange(0, 2);  // 0=데미지, 1=사거리, 2=사속  

		switch (OptionType)
		{
		case 0: // 데미지
			Offer.StatType = EP9ShopStatType::Damage;
			Offer.DamageBonus = GetDamageBonusByRarity(Offer.Rarity);
			Offer.RangeBonus = 0.f;
			Offer.FireSpeedBonus = 0.f;
			break;

		case 1: // 사거리
			Offer.StatType = EP9ShopStatType::Range;
			Offer.DamageBonus = 0.f;
			Offer.RangeBonus = GetRangeBonusByRarity(Offer.Rarity);
			Offer.FireSpeedBonus = 0.f;
			break;

		case 2: // 발사속도
			Offer.StatType = EP9ShopStatType::FireSpeed;
			Offer.DamageBonus = 0.f;
			Offer.RangeBonus = 0.f;
			Offer.FireSpeedBonus = GetFireSpeedBonusByRarity(Offer.Rarity);
			break;
		}

		CurrentOffers.Add(Offer);
	}

	bOffersBuilt = true;
}
//요기도
FShopOffer AP9Shop::GetOffer(int32 Index) const
{
	if (CurrentOffers.IsValidIndex(Index))
	{
		return CurrentOffers[Index];
	}

	return FShopOffer();
}

bool AP9Shop::TryPurchaseWithCurrentPawn(int32 OfferIndex)
{
	return TryPurchase(OfferIndex, OverlappedPawn);
}

bool AP9Shop::PickThreeDistinctWeapons(TArray<FName>& OutWeaponIds) const
{
	OutWeaponIds.Reset();
	if (!WeaponDataTable) return false;

	const TArray<FName> Rows = WeaponDataTable->GetRowNames();
	if (Rows.Num() < 3) return false;

	TArray<FName> Pool = Rows;
	for (int32 i = 0; i < 16; ++i)
	{
		const int32 A = FMath::RandRange(0, Pool.Num() - 1);
		const int32 B = FMath::RandRange(0, Pool.Num() - 1);
		if (A != B) Pool.Swap(A, B);
	}
	OutWeaponIds.Append(Pool.GetData(), 3);
	return true;
}

EP9ShopRarity AP9Shop::RollRarity() const
{
	const float r = FMath::FRand();
	float acc = Prob_Common;
	if (r < acc) return EP9ShopRarity::Common;

	acc += Prob_Uncommon;
	if (r < acc) return EP9ShopRarity::Uncommon;

	acc += Prob_Rare;
	if (r < acc) return EP9ShopRarity::Rare;

	return EP9ShopRarity::Legendary;
}

int32 AP9Shop::GetPriceByRarity(EP9ShopRarity Rarity) const
{
	switch (Rarity)
	{
	case EP9ShopRarity::Common:     return Price_Common;
	case EP9ShopRarity::Uncommon:   return Price_Uncommon;
	case EP9ShopRarity::Rare:       return Price_Rare;
	case EP9ShopRarity::Legendary:  return Price_Legendary;
	default:                        return Price_Common;
	}
}

float AP9Shop::GetDamageBonusByRarity(EP9ShopRarity Rarity) const
{
	switch (Rarity)
	{
	case EP9ShopRarity::Common:     return 3.f;
	case EP9ShopRarity::Uncommon:   return 6.f;
	case EP9ShopRarity::Rare:       return 9.f;
	case EP9ShopRarity::Legendary:  return 12.f;
	default:                        return 0.f;
	}
}

float AP9Shop::GetRangeBonusByRarity(EP9ShopRarity Rarity) const
{
	switch (Rarity)
	{
	case EP9ShopRarity::Common:     return 50.f;
	case EP9ShopRarity::Uncommon:   return 100.f;
	case EP9ShopRarity::Rare:       return 150.f;
	case EP9ShopRarity::Legendary:  return 200.f;
	default:                        return 0.f;
	}
}

float AP9Shop::GetFireSpeedBonusByRarity(EP9ShopRarity Rarity) const
{
	switch (Rarity)
	{
	case EP9ShopRarity::Common:     return -0.1f;
	case EP9ShopRarity::Uncommon:   return -0.2f;
	case EP9ShopRarity::Rare:       return -0.3f;
	case EP9ShopRarity::Legendary:  return -0.4f;
	default:                        return 0.f;
	}
}

// 구매: 골드 차감 + 인벤토리 등록
bool AP9Shop::TryPurchase(int32 OfferIndex, APawn* BuyerPawn)
{
	if (!BuyerPawn) return false;
	if (!CurrentOffers.IsValidIndex(OfferIndex)) return false;

	const FShopOffer& Offer = CurrentOffers[OfferIndex];

	// PlayerState(골드/보정 관리)
	AP9PlayerState* PS = BuyerPawn->GetPlayerState<AP9PlayerState>();
	if (!PS) return false;

	// 골드 확인
	if (!PS->CanAfford(Offer.Price))
	{
		return false;
	}

	// 결제
	if (!PS->SpendGold(Offer.Price))
	{
		return false;
	}

	UP9InventoryComponent* Inv = BuyerPawn->FindComponentByClass<UP9InventoryComponent>();
	if (!Inv) return false;

	if (!Inv->AddWeaponById_Validated(Offer.WeaponId))
	{
		return false;
	}

	//  데미지 
	if (!FMath::IsNearlyZero(Offer.DamageBonus))
	{
		PS->AddWeaponDamageBonus(Offer.WeaponId, Offer.DamageBonus);
	}

	//  사거리 
	if (!FMath::IsNearlyZero(Offer.RangeBonus))
	{
		PS->AddWeaponRangeBonus(Offer.WeaponId, Offer.RangeBonus);
	}

	//  발사속도
	if (!FMath::IsNearlyZero(Offer.FireSpeedBonus))
	{
		PS->AddWeaponFireSpeedBonus(Offer.WeaponId, Offer.FireSpeedBonus);
	}

	//구매 성공 후 사라지기
	if (AP9GameMode* GM = Cast<AP9GameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->OnShopPurchased();
	}

	return true;
}

void AP9Shop::OnTriggerBegin(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp,
	int32 BodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	if (!Other || !Other->IsA<APawn>()) return;

	OverlappedPawn = Cast<APawn>(Other);

	bPlayerInRange = true;
	BindInput();
}

void AP9Shop::OnTriggerEnd(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp,
	int32 BodyIndex)
{
	if (!Other || !Other->IsA<APawn>()) return;

	if (Other == OverlappedPawn)
	{
		OverlappedPawn = nullptr;
	}

	bPlayerInRange = false;
	UnbindInput();
}

void AP9Shop::BindInput()
{
	if (!CachedPC) return;

	EnableInput(CachedPC);

	if (InputComponent && !bInteractBound)
	{
		InputComponent->BindKey(EKeys::E, IE_Pressed, this, &AP9Shop::HandleInteract);
		bInteractBound = true;
	}
}

void AP9Shop::UnbindInput()
{
	if (!CachedPC) return;

	if (InputComponent)
	{
		InputComponent->ClearActionBindings();
		bInteractBound = false;
	}
	DisableInput(CachedPC);
}

void AP9Shop::HandleInteract()
{
	if (!bPlayerInRange) return;
	BuildOffers();
	OnPressE.Broadcast();
}
