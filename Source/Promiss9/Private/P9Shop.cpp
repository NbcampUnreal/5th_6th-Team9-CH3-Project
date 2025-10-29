#include "P9Shop.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

AP9Shop::AP9Shop()
{
    PrimaryActorTick.bCanEverTick = false;

    // 캡슐
    Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
    Capsule->InitCapsuleSize(34.f, 88.f);
    Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Capsule->SetCollisionResponseToAllChannels(ECR_Block);
    RootComponent = Capsule;

    // 스켈레탈 메쉬
    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetRelativeLocation(FVector(0.f, 0.f, -88.f)); 
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 말걸 범위
    TalkSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TalkSphere"));
    TalkSphere->SetupAttachment(RootComponent);
    TalkSphere->InitSphereRadius(180.f);
    TalkSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TalkSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    TalkSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    // 오버랩 바인딩
    TalkSphere->OnComponentBeginOverlap.AddDynamic(this, &AP9Shop::OnTalkBeginOverlap);
    TalkSphere->OnComponentEndOverlap.AddDynamic(this, &AP9Shop::OnTalkEndOverlap);
}

void AP9Shop::BeginPlay()
{
    Super::BeginPlay();
}

void AP9Shop::OnTalkBeginOverlap(UPrimitiveComponent* /*OverlappedComp*/, AActor* OtherActor,
    UPrimitiveComponent* /*OtherComp*/, int32 /*OtherBodyIndex*/, bool /*bFromSweep*/, const FHitResult& /*Sweep*/)
{
    if (!OtherActor) return;

    //  캐릭터만 상호작용 대상
    if (Cast<ACharacter>(OtherActor))
    {
        bPlayerInRange = true;
        CachedInteractor = OtherActor;

        UE_LOG(LogTemp, Log, TEXT("[%s] Player entered talk range."), *GetName());
    }
}

void AP9Shop::OnTalkEndOverlap(UPrimitiveComponent* /*OverlappedComp*/, AActor* OtherActor,
    UPrimitiveComponent* /*OtherComp*/, int32 /*OtherBodyIndex*/)
{
    if (!OtherActor) return;

    if (CachedInteractor.Get() == OtherActor)
    {
        bPlayerInRange = false;
        CachedInteractor = nullptr;

        UE_LOG(LogTemp, Log, TEXT("[%s] Player left talk range."), *GetName());
    }
}

void AP9Shop::Interact(AActor* Interactor)
{
    if (!Interactor) return;

    if (bPlayerInRange)
    {
        UE_LOG(LogTemp, Log, TEXT("[%s] Interact by %s"), *GetName(), *Interactor->GetName());
        OpenShop(Interactor);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] Interact called but player not in range."), *GetName());
    }
}
