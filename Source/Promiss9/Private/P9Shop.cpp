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
		Offer.DamageBonus = GetDamageBonusByRarity(Offer.Rarity);
		CurrentOffers.Add(Offer);
	}
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

int32 AP9Shop::GetDamageBonusByRarity(EP9ShopRarity Rarity) const
{
	switch (Rarity)
	{
	case EP9ShopRarity::Common:     return 3;
	case EP9ShopRarity::Uncommon:   return 6;
	case EP9ShopRarity::Rare:       return 9;
	case EP9ShopRarity::Legendary:  return 12;
	default:                        return 0;
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
#if !(UE_BUILD_SHIPPING)
		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Red, TEXT("골드 부족"));
#endif
		return false;
	}

	UP9InventoryComponent* Inv = BuyerPawn->FindComponentByClass<UP9InventoryComponent>();
	if (!Inv) return false;

	if (!Inv->AddWeaponById_Validated(Offer.WeaponId))
	{
		return false;
	}

	// 결제
	if (!PS->SpendGold(Offer.Price))
	{
		return false;
	}

	const int32 FlatBonus = GetDamageBonusByRarity(Offer.Rarity);
	if (FlatBonus != 0)
	{
		PS->AddWeaponDamageBonus(Offer.WeaponId, Offer.DamageBonus);
	}

#if !(UE_BUILD_SHIPPING)
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
		FString::Printf(TEXT("구매 성공: %s / %s / -%d Gold ( +%d)"),
			*Offer.WeaponId.ToString(),
			*UEnum::GetValueAsString(Offer.Rarity),
			Offer.Price, FlatBonus));
#endif

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
#if !(UE_BUILD_SHIPPING)
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, TEXT("E : 상점 열기"));
#endif
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
