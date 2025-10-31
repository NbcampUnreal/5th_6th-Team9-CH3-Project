#include "P9FinalBossAltar.h"
#include "P9Character.h"



AP9FinalBossAltar::AP9FinalBossAltar()
{
	PrimaryActorTick.bCanEverTick = false;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	BossSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("BossSpawnPoint"));
	BossSpawnPoint->SetupAttachment(RootComponent);

	Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerZone"));

	Trigger->SetupAttachment(RootComponent);

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AP9FinalBossAltar::OverlapBegin);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &AP9FinalBossAltar::OverlapEnd);
}

void AP9FinalBossAltar::BeginPlay()
{
	Super::BeginPlay();
	
}


void AP9FinalBossAltar::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AP9Character* Player = Cast<AP9Character>(OtherActor);

	if (Player != nullptr)
	{
		Player->CurrentOverlappingAltar = this;
	}
}

void AP9FinalBossAltar::OverlapEnd(UPrimitiveComponent* OvelappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AP9Character* Player = Cast<AP9Character>(OtherActor);

	if (Player != nullptr)
	{
		Player->CurrentOverlappingAltar = nullptr;
	}
}

void AP9FinalBossAltar::InteractionTimerComplete()
{
	if (FinalBoss == nullptr) return;
	if (BossSpawnPoint == nullptr) return;

	GetWorld()->SpawnActor<AP9Monster>(FinalBoss, BossSpawnPoint->GetComponentLocation(), BossSpawnPoint->GetComponentRotation());
}
