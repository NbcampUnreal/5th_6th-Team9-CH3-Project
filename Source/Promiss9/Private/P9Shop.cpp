// P9Shop.cpp
#include "P9Shop.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

#include "P9PlayerState.h"          // CanAfford(), SpendGold()
#include "P9InventoryComponent.h"   // AddWeaponById_Validated()
#include "P9WeaponData.h"           // FP9WeaponData (RowStruct)

// -------------------------------------------------------

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

// 무기 3개(중복X) + 희귀도 랜덤 + 가격 설정
void AP9Shop::BuildOffers()
{
	CurrentOffers.Reset();

	TArray<FName> Picked;
	if (!PickThreeDistinctWeapons(Picked))
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("[Shop] DataTable이 없거나 Row가 3개 미만"));
		return;
	}

	for (int32 i = 0; i < Picked.Num(); ++i)
	{
		FShopOffer Offer;
		Offer.WeaponId = Picked[i];
		Offer.Rarity = RollRarity();
		Offer.Price = GetPriceByRarity(Offer.Rarity);

		CurrentOffers.Add(Offer);

		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow,
			FString::Printf(TEXT("[Offer %d] %s / %s / %d Gold"),
				i + 1,
				*Offer.WeaponId.ToString(),
				*UEnum::GetValueAsString(Offer.Rarity),
				Offer.Price));
	}

}

// DataTable에서 중복 없이 3개 뽑기
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

// 희귀도 
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

// 희귀도별 가격
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

// 구매: 골드 차감 + 인벤토리 추가
bool AP9Shop::TryPurchase(int32 OfferIndex, APawn* BuyerPawn)
{
	if (!BuyerPawn) return false;
	if (!CurrentOffers.IsValidIndex(OfferIndex)) return false;

	const FShopOffer& Offer = CurrentOffers[OfferIndex];

	// PlayerState(골드)
	AP9PlayerState* PS = BuyerPawn->GetPlayerState<AP9PlayerState>();
	if (!PS)
	{
		return false;
	}

	// 골드 확인 
	if (!PS->CanAfford(Offer.Price))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Red, TEXT("골드 부족"));
		return false;
	}

	// 인벤토리
	UP9InventoryComponent* Inv = BuyerPawn->FindComponentByClass<UP9InventoryComponent>();
	if (!Inv)
	{
		return false;
	}

	// DataTable 검증 후 추가
	if (!Inv->AddWeaponById_Validated(Offer.WeaponId))
	{
		return false;
	}

	// 결제
	if (!PS->SpendGold(Offer.Price))
	{
		return false;
	}

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
		FString::Printf(TEXT("구매 성공: %s / %s / -%d Gold"),
			*Offer.WeaponId.ToString(),
			*UEnum::GetValueAsString(Offer.Rarity),
			Offer.Price));

	return true;
}

// 오버랩 & 입력

void AP9Shop::OnTriggerBegin(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp,
	int32 BodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	if (!Other || !Other->IsA<APawn>()) return;

	bPlayerInRange = true;
	GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Cyan, TEXT("E : 상점 열기"));

	BindInput();
}

void AP9Shop::OnTriggerEnd(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp,
	int32 BodyIndex)
{
	if (!Other || !Other->IsA<APawn>()) return;

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
	BuildOffers(); // 매번 갱신(랜덤)
}

