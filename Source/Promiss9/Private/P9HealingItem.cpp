// Fill out your copyright notice in the Description page of Project Settings.


#include "P9HealingItem.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Components/PrimitiveComponent.h"



AP9HealingItem::AP9HealingItem()
{	
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetupAttachment(GetRootComponent()); 

	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collision->SetCollisionObjectType(ECC_WorldDynamic);
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Collision->SetGenerateOverlapEvents(true);

	
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AP9HealingItem::OnItemOverlap);
	//Collision->OnComponentEndOverlap.AddDynamic(this, &AP9HealingItem::OnItemEndOverlap);

	PrimaryActorTick.bCanEverTick = true;
	RotationSpeed = 90.0f;
}

void AP9HealingItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(!FMath::IsNearlyZero(RotationSpeed))
	{
		AddActorLocalRotation(FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f));
	}
}

void AP9HealingItem::OnItemOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	// Implement overlap logic here
	if (AP9Character* PlayerCharacter = Cast<AP9Character>(OtherActor))
	{
		if (APawn* Pawn = Cast<APawn>(OtherActor))
		{
			if(Pawn->IsPlayerControlled())
			{
				ActivateItem(PlayerCharacter);
			}
		}
	}
}

void AP9HealingItem::OnItemEndOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
}

void AP9HealingItem::ActivateItem(AActor* Activator)
{
	UParticleSystemComponent* Particle = nullptr;

	if (ParticleEffect)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ParticleEffect,
			GetActorLocation(),
			GetActorRotation(),
			true
		);
	}

	if (Particle)
	{
		FTimerHandle DestroyParticleTimerHandle;
		TWeakObjectPtr<UParticleSystemComponent> WeakParticle = Particle;

		GetWorld()->GetTimerManager().SetTimer(
			DestroyParticleTimerHandle,
			[WeakParticle]()
			{
				if (WeakParticle.IsValid())
				{
					WeakParticle->DestroyComponent();
				}
			},
			2.0f,
			false
		);
	}
	if(AP9Character* PlayerCharacter = Cast<AP9Character>(Activator))
	{
		applyHeal(PlayerCharacter, HealAmount);
		DestroyItem();
	}
}

FName AP9HealingItem::GetItemType()const
{
	return ItemType;
}

void AP9HealingItem::DestroyItem()
{
	Destroy();
}

void AP9HealingItem::applyHeal(AActor* Target, float InAmount)
{
	if (!Target || InAmount <= 0.0f)
	{
		return;
	}
	if(AP9Character* PlayerCharacter = Cast<AP9Character>(Target))
	{
		float MaxHealth = PlayerCharacter->GetMaxHealth();
		float HealPoint = MaxHealth * InAmount;
		PlayerCharacter->AddHealth(HealPoint);
	}
}

// ===== 자동 드랍 풀(파생 BP만, 로드 없이 경로로 수집) =====
static TArray<TSoftClassPtr<AP9HealingItem>>& GetHealingItemPool()
{
	static bool bBuilt = false;
	static TArray<TSoftClassPtr<AP9HealingItem>> Pool;
	if (bBuilt) return Pool;
	bBuilt = true;

	IAssetRegistry& AR = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();

	TArray<FTopLevelAssetPath> BaseClasses;
	BaseClasses.Add(AP9HealingItem::StaticClass()->GetClassPathName()); // UE5.3+: ClassPaths

	TSet<FTopLevelAssetPath> Excluded;
	TSet<FTopLevelAssetPath> OutDerived;
	AR.GetDerivedClassNames(BaseClasses, Excluded, OutDerived); // 파생 클래스 경로만 추출

	for (const FTopLevelAssetPath& Path : OutDerived)
	{
		FSoftClassPath SCP(Path.ToString());
		Pool.Add(TSoftClassPtr<AP9HealingItem>(SCP)); // 로드 지연(Soft)
	}
	return Pool;
}

// ===== 가중치 랜덤 선택 (CDO 검사 포함) =====
static UClass* RandomHealingItemBP()
{
	const auto& Pool = GetHealingItemPool();
	if (Pool.Num() == 0)
	{
		return nullptr;
	}
	struct FCand { UClass* Cls; double W; const AP9HealingItem* CDO; };
	TArray<FCand> Cands; Cands.Reserve(Pool.Num());
	double TotalW = 0.0;

	for (const auto& Soft : Pool)
	{
		if (UClass* C = Soft.LoadSynchronous())
		{
			if (!C->IsChildOf(AP9HealingItem::StaticClass()))
			{
				continue;
			}
			if (!C->HasAnyClassFlags(CLASS_CompiledFromBlueprint))
			{
				continue;
			}
			if (C->HasAnyClassFlags(CLASS_Abstract))
			{
				continue;
			}
			const AP9HealingItem* CDO = Cast<AP9HealingItem>(C->GetDefaultObject());
				if (!CDO)
				{
					continue;
				}
				if(!CDO->bEligibleForDrop || CDO->DropWeight <= 0.0f)
				{
					continue;
				}
				Cands.Add({ C,(double)CDO->DropWeight, CDO });
				TotalW += CDO->DropWeight;
		
		}
	}
	if (Cands.Num() == 0 || TotalW <= 0.0)
	{
		return nullptr;
	}
	double r = FMath::FRand() * TotalW;
	for (const FCand& E : Cands)
	{
		r -= E.W;
		if (r <= 0.0) return E.Cls;
	}
	return Cands.Last().Cls; // 안전장치
}

// ===== 스폰 (★ DropChance/Offset/Impulse 모두 "선택된 BP의 CDO"에서 사용) =====
AP9HealingItem* AP9HealingItem::SpawnHealingItem(const UObject* WorldContextObject, const AActor* SourceActor)
{
	if (!WorldContextObject || !SourceActor)
	{
		return nullptr;
	}
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (!World)
	{
		return nullptr;
	}
	// 1) 어떤 힐 아이템 BP를 드랍할지 선택
	UClass* ChosenClass = RandomHealingItemBP();
	if (!ChosenClass)
	{
		return nullptr;
	}
	// 2) ★ 선택된 BP의 CDO에서 값을 읽는다 (BP 값이 반영됨)
	const AP9HealingItem* CDO = Cast<AP9HealingItem>(ChosenClass->GetDefaultObject());
	if (!CDO)
	{
		return nullptr;
	}
	// 3) 확률 체크: BP에서 DropChance를 1.0으로 바꾸면 100% 드랍
	float Text = FMath::FRand();
	if (Text >= CDO->DropChance)
	{
		return nullptr;
	}
	const FVector SpawnLocation = SourceActor->GetActorLocation() + CDO->DropOffset;
	const FRotator SpawnRotation = FRotator::ZeroRotator;

	AP9HealingItem* Item = Cast<AP9HealingItem>(
		UGameplayStatics::BeginDeferredActorSpawnFromClass(World, ChosenClass, FTransform(SpawnRotation, SpawnLocation)));
	if (!Item)
	{
		return nullptr;
	}
	UGameplayStatics::FinishSpawningActor(Item, FTransform(SpawnRotation, SpawnLocation));

	// 임펄스도 BP 값 사용
	if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(Item->GetRootComponent()))
	{
		if (Root->IsSimulatingPhysics())
		{
			const FVector Dir = FVector(FMath::FRandRange(-1.0f, 1.0f), FMath::FRandRange(-1.0f, 1.0f), 1.0f).GetSafeNormal();
			Root->AddImpulse(Dir * CDO->DropImpulse, NAME_None, true);
		}
	}
	return Item;
}
