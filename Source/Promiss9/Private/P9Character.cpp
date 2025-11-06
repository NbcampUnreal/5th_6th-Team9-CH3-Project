// Fill out your copyright notice in the Description page of Project Settings.

#include "P9Character.h"
#include "P9PlayerController.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Blueprint/UserWidget.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AP9Character::AP9Character()
{
	PrimaryActorTick.bCanEverTick = true;

	//카메라
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 500.0f;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(RootComponent);
	SphereCollision->InitSphereRadius(150.0f);
	SphereCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic")); // 충돌 설정
	SphereCollision->SetGenerateOverlapEvents(true); // 오버랩 이벤트 활성화

	WaeponSocket_fr = CreateDefaultSubobject<USceneComponent>(TEXT("Socket_fr"));
	WaeponSocket_fr->SetupAttachment(GetMesh());
	WaeponSocket_fl = CreateDefaultSubobject<USceneComponent>(TEXT("Socket_fl"));
	WaeponSocket_fl->SetupAttachment(GetMesh());
	WaeponSocket_rr = CreateDefaultSubobject<USceneComponent>(TEXT("Socket_rr"));
	WaeponSocket_rr->SetupAttachment(GetMesh());
	WaeponSocket_rl = CreateDefaultSubobject<USceneComponent>(TEXT("Socket_rl"));
	WaeponSocket_rl->SetupAttachment(GetMesh());

	// 신 컴포넌트 위치, 회전 조정(캐릭터 Mesh를 중심으로 4방향)
	WaeponSocket_fr->SetRelativeLocation(FVector(-80.f, 80.0f, 210.0f));
	WaeponSocket_fr->SetRelativeRotation(FRotator(0.0f, 135.0f, 0.0f));

	WaeponSocket_fl->SetRelativeLocation(FVector(80.0f, 80.0f, 210.0f));
	WaeponSocket_fl->SetRelativeRotation(FRotator(0.0f, 45.0f, 0.0f));

	WaeponSocket_rr->SetRelativeLocation(FVector(-80.0f, -80.0f, 210.0f));
	WaeponSocket_rr->SetRelativeRotation(FRotator(0.0f, -135.0f, 0.0f));

	WaeponSocket_rl->SetRelativeLocation(FVector(80.0f, -80.0f, 210.0f));
	WaeponSocket_rl->SetRelativeRotation(FRotator(0.0f, -45.0f, 0.0f));

	// StaticMeshComponent 생성, 부착
	WeaponMesh_fr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh_fr"));
	WeaponMesh_fr->SetupAttachment(WaeponSocket_fr);
	WeaponMesh_fl = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh_fl"));
	WeaponMesh_fl->SetupAttachment(WaeponSocket_fl);
	WeaponMesh_rr = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh_rr"));
	WeaponMesh_rr->SetupAttachment(WaeponSocket_rr);
	WeaponMesh_rl = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh_rl"));
	WeaponMesh_rl->SetupAttachment(WaeponSocket_rl);

	// 인벤토리 컴포넌트
	InventoryComponent = CreateDefaultSubobject<UP9InventoryComponent>(TEXT("InventoryComponent"));

	bIsFreeLookMode = false;
	SavedArmLength = 0.0f;

	// 이동 관련
	NormalSpeed = 300.0f;
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	// 회전 관련
	GetCharacterMovement()->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = false;
	DefaultYaw = 0.f;
	TargetYawOffset = 0.f;
	RotationInterpSpeed = 8.0f;
	bIsSideMoving = false;

	// 앞구르기 관련
	bCanRoll = true;
	RollDistanceTraveled = 0.0f;
	RollSpeed = 1.0f; // 속도
	RollTargetDistance = 1000.0f; // 거리
	bForwardRolling = false;

	// Health
	MaxHealth = 100.0f;
	Health = MaxHealth;
}

void AP9Character::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	if (TargetActor)
	{
		RotateMeshToTarget(TargetActor);
	}

	EquipWeaponToRightHandSockets();
}

// Called every frame
void AP9Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Controller) return;

	if (bForwardRolling)
	{
		FVector LaunchDirection = GetActorForwardVector().GetSafeNormal();
		LaunchCharacter(LaunchDirection * RollSpeed, true, true);

		FVector DesiredVelocity = GetActorForwardVector().GetSafeNormal() * RollSpeed;
		FVector NewVelocity = GetCharacterMovement()->Velocity + DesiredVelocity * 0.016f; // 단순 보정용, FPS 60 기준
		float MaxRollSpeed = 1.f;
		if (NewVelocity.Size() > MaxRollSpeed)
		{
			NewVelocity = NewVelocity.GetSafeNormal() * MaxRollSpeed;
		}
		GetCharacterMovement()->Velocity = NewVelocity;

		return;
	}

	if (!bIsFreeLookMode)
	{
		// 컨트롤러(카메라) 방향 기준으로 몸 회전
		FRotator ControlRot = Controller->GetControlRotation();
		ControlRot.Pitch = 0.f;
		ControlRot.Roll = 0.f;

		TargetYawOffset = FMath::FInterpTo(TargetYawOffset, 0.f, DeltaTime, RotationInterpSpeed);

		FRotator TargetRot = ControlRot;
		TargetRot.Yaw += TargetYawOffset;

		// 부드럽게 회전 보간
		FRotator NewRot = FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, RotationInterpSpeed);
		SetActorRotation(NewRot);
	}
}

void AP9Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AP9PlayerController* PlayerController = Cast<AP9PlayerController>(GetController()))
		{
			// 이동 입력 바인딩
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&AP9Character::Move
				);

				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Completed,
					this,
					&AP9Character::MoveCompleted
				);
			}

			// 점프 입력 바인딩
			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&AP9Character::StartJump
				);

				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&AP9Character::StopJump
				);
			}

			// Look
			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&AP9Character::Look
				);
			}

			//Zoom
			if (PlayerController->ZoomCameraAction)
			{
				EnhancedInput->BindAction(
					PlayerController->ZoomCameraAction,
					ETriggerEvent::Triggered,
					this,
					&AP9Character::ZoomCamera
				);
			}

			//FreeLook
			if (PlayerController->FreeLookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->FreeLookAction,
					ETriggerEvent::Started,
					this,
					&AP9Character::OnFreeLookStart
				);

				EnhancedInput->BindAction(
					PlayerController->FreeLookAction,
					ETriggerEvent::Completed,
					this,
					&AP9Character::OnFreeLookEnd
				);

			}

			// ForwardRoll
			if (PlayerController->ForwardRollAction)
			{
				EnhancedInput->BindAction(
					PlayerController->ForwardRollAction,
					ETriggerEvent::Started,
					this,
					&AP9Character::StartForwardRoll
				);

				//EnhancedInput->BindAction(
				//	PlayerController->ForwardRollAction,
				//	ETriggerEvent::Completed,
				//	this,
				//	&AP9Character::StopForwardRoll
				//);
			}

			// Interact
			if (PlayerController->InteractAction)
			{
				EnhancedInput->BindAction(
					PlayerController->InteractAction,
					ETriggerEvent::Started,
					this,
					&AP9Character::InteractPressed
				);

				EnhancedInput->BindAction(
					PlayerController->InteractAction,
					ETriggerEvent::Completed,
					this,
					&AP9Character::InteractReleased
				);
			}
		}
	}
}

void AP9Character::Move(const FInputActionValue& Value)
{
	if (!Controller) return;

	if (bForwardRolling) return;

	FVector2D MoveInput = Value.Get<FVector2D>();
	float ForwardValue = MoveInput.X;
	float SideValue = MoveInput.Y;

	FRotator ControlRot = Controller->GetControlRotation();
	ControlRot.Pitch = 0.f;
	ControlRot.Roll = 0.f;


	if (!FMath::IsNearlyZero(ForwardValue))
	{
		AddMovementInput(FRotationMatrix(ControlRot).GetUnitAxis(EAxis::X), ForwardValue);
		TargetYawOffset = 0.f;
	}
	if (!FMath::IsNearlyZero(SideValue))
	{
		bIsSideMoving = true;

		// 왼쪽
		if (SideValue < 0)
		{
			TargetYawOffset = -90.f;
		}

		// 오른쪽
		else if (SideValue > 0)
		{
			TargetYawOffset = 90.f;
		}

		const FRotator MoveRot(0.f, ControlRot.Yaw + TargetYawOffset, 0.f);
		FVector MoveDir = FRotationMatrix(MoveRot).GetUnitAxis(EAxis::X);
		AddMovementInput(MoveDir, 1.f);
	}
}

void AP9Character::MoveCompleted(const FInputActionValue& Value)
{
	// 좌우 입력이 끝나면 정면 복귀
	if (bIsSideMoving)
	{
		TargetYawOffset = 0.f;
		bIsSideMoving = false;
	}
}

void AP9Character::StartJump(const FInputActionValue& Value)
{
	// 앞구르기 중 무시
	if (bForwardRolling) return;

	if (Value.Get<bool>())
	{
		Jump();
	}
}

void AP9Character::StopJump(const FInputActionValue& Value)
{
	if (bForwardRolling) return;

	if (Value.Get<bool>())
	{
		StopJumping();
	}
}

void AP9Character::Look(const FInputActionValue& Value)
{
	FVector2D LookInput = Value.Get<FVector2D>();

	if (!Controller) return;

	if (bIsFreeLookMode)
	{
		// 자유시점 중엔 컨트롤러만 회전 변경
		AddControllerYawInput(LookInput.X);
		AddControllerPitchInput(LookInput.Y);
	}
	else
	{
		// 일반 모드 카메라, 캐릭터 모두 회전
		AddControllerYawInput(LookInput.X);
		AddControllerPitchInput(LookInput.Y);
	}
}

void AP9Character::ZoomCamera(const FInputActionValue& Value)
{
	float NewLength = FMath::Clamp(SpringArmComp->TargetArmLength + Value.Get<float>() * 100.0f, 200.0f, 1100.0f);
	SpringArmComp->TargetArmLength = NewLength;
}

void AP9Character::OnFreeLookStart(const FInputActionValue& Value)
{
	bIsFreeLookMode = true;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	SavedControlRotation = Controller->GetControlRotation();

	UE_LOG(LogTemp, Warning, TEXT(">> FreeLook Start"));
}

void AP9Character::OnFreeLookEnd(const FInputActionValue& Value)
{
	bIsFreeLookMode = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	if (Controller)
	{
		Controller->SetControlRotation(SavedControlRotation);
	}

	UE_LOG(LogTemp, Warning, TEXT(">> FreeLook End"));
}

// 수정
void AP9Character::StartForwardRoll()
{
	if (bForwardRolling || !bCanRoll || !ForwardRollMontage || GetCharacterMovement()->IsFalling()) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		bForwardRolling = true;
		bCanRoll = false;
		RollDistanceTraveled = 0.0f;
		// 수정
		RollSpeed = 1500.0f; // 속도
		RollTargetDistance = 1500.0f; // 거리

		// 무기 안보기에 하기
		HideAllWeapons(true);

		// 애니메이션 몽타주 재생
		UE_LOG(LogTemp, Warning, TEXT("몽타주 실행"));

		OriginalMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
		GetWorldTimerManager().SetTimer(StopMovementTimerHandle, this, &AP9Character::StopMovementCompletely, RollDuration, false);

		AnimInstance->Montage_Play(ForwardRollMontage, RollPlayRate);



		RollMontageEndedDelegate.BindUObject(this, &AP9Character::OnRollMontageEnded);
		AnimInstance->Montage_SetEndDelegate(RollMontageEndedDelegate, ForwardRollMontage);

		GetWorldTimerManager().SetTimer(RollCooldownTimerHandle, this, &AP9Character::ResetRollCooldown, 2.0f, false);
	}
}

void AP9Character::StopMovementCompletely()
{
	UE_LOG(LogTemp, Warning, TEXT("움직임 제어"));

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	StopForwardRoll();
}

void AP9Character::OnRollMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 다른 몽타주와 섞이지 않게 확인
	if (Montage != ForwardRollMontage) return;

	UE_LOG(LogTemp, Warning, TEXT("bForwardRolling = false"));
	HideAllWeapons(false);

	GetWorldTimerManager().SetTimer(
		RollMovementRestoreTimerHandle,
		this,
		&AP9Character::RestoreMovementAfterRoll,
		0.1f,
		false
	);

	if (bInterrupted)
	{
		GetCharacterMovement()->StopMovementImmediately();
	}
}

void AP9Character::RestoreMovementAfterRoll()
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

void AP9Character::StopForwardRoll()
{
	bForwardRolling = false;
}

void AP9Character::ResetRollCooldown()
{
	bCanRoll = true;
}

float AP9Character::GetHealth() const
{
	return Health;
}

void AP9Character::SetHealth(float NewHealth)
{
	Health = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
}


void AP9Character::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
}


float AP9Character::GetMaxHealth() const
{
	return MaxHealth;
}

void AP9Character::SetMaxHealth(float NewMaxHealth)
{
	MaxHealth = FMath::Max(NewMaxHealth, 0.0f);
}

void AP9Character::AddMaxHealth(float Amount)
{
	float Add = (MaxHealth) * (Amount / 100);
	MaxHealth += Add;
	Health += Add;
	MaxHealth = FMath::Max(MaxHealth, 0.0f);
}


float AP9Character::GetNormalSpeed() const
{
	return NormalSpeed;
}

void AP9Character::SetNormalSpeed(float NewNormalSpeed)
{
	NormalSpeed = NewNormalSpeed;
}

// 걷기 속도 입력 함수 수정
void AP9Character::AddNormalSpeed(float Amount)
{
	NormalSpeed += (300) * (Amount/100);
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
}

float AP9Character::TakeDamage(
	float DamageAmount,
	struct FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	// 애니메이션 몽타주 실행 여부
	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	bool bIsRollingMontagePlaying = false;

	if (AnimInstance && ForwardRollMontage)
	{
		bIsRollingMontagePlaying = AnimInstance->Montage_IsPlaying(ForwardRollMontage);
	}
	
	// CharacterLaunch 여부
	bool bIsLaunched = !GetCharacterMovement()->IsMovingOnGround();

	//구르기 중이거나 / 롤 몽타주 중이거나 / LaunchCharacter 이동 중일 때는 데미지 무시
	if (bForwardRolling || bIsRollingMontagePlaying || bIsLaunched)
	{
		UE_LOG(LogTemp, Warning, TEXT(">> Damage ignored (Rolling=%d, Montage=%d, Launched=%d)"),
			bForwardRolling, bIsRollingMontagePlaying, bIsLaunched);
		return 0.f;
	}

	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);

	if (Health <= 0.0f)
	{
		OnDeath.Broadcast();
	}

	return ActualDamage;
}

// 오른손 무기
void AP9Character::EquipWeaponToRightHandSockets()
{
	if (!HandWeaponClass) return;

	UWorld* World = GetWorld();
	if (!World) return;

	// 원하는 소켓 이름 목록
	TArray<FName> SocketNames = { TEXT("Weapon_rh") };

	for (FName SocketName : SocketNames)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		FVector SpawnLocation = GetActorLocation();
		FRotator SpawnRotation = GetActorRotation();

		// 무기 액터 생성
		AActor* SpawnedWeapon = World->SpawnActor<AActor>(HandWeaponClass, SpawnLocation, SpawnRotation, SpawnParams);
		if (!SpawnedWeapon) continue;

		// 캐릭터 메시에 부착
		USkeletalMeshComponent* MeshComp = GetMesh();
		if (!MeshComp) continue;

		SpawnedWeapon->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);

		// 참조 저장
		EquippedWeapons.Add(SpawnedWeapon);

		UE_LOG(LogTemp, Warning, TEXT("Weapon spawned: %s"), *SpawnedWeapon->GetName());
	}
}

// 무기 숨기기
void AP9Character::HideAllWeapons(bool bHide)
{

	for (AActor* Weapon : EquippedWeapons)
	{
		if (!Weapon) continue;

		// 렌더링 숨기기
		Weapon->SetActorHiddenInGame(bHide);

		// 충돌 제거
		TArray<UActorComponent*> MeshComponents;
		Weapon->GetComponents(UPrimitiveComponent::StaticClass(), MeshComponents);
		for (UActorComponent* Comp : MeshComponents)
		{
			UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Comp);
			if (Primitive)
			{
				Primitive->SetCollisionEnabled(bHide ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);
			}
		}
	}
}

void AP9Character::ApplyPenaltyDamage(float DamageAmount)
{
	if (Health <= 0.0f || DamageAmount <= 0.0f) return;

	Health -= DamageAmount;
	Health = FMath::Clamp(Health, 0.0f, MaxHealth);

	if (Health <= 0.0f)
	{
		OnDeath.Broadcast();
	}
}

void AP9Character::InteractPressed()
{
	if (CurrentOverlappingAltar != nullptr)
	{
		GetWorldTimerManager().SetTimer(InteractHoldTimer, this, &AP9Character::InteractHoldSucceeded, CurrentOverlappingAltar->InteractionDuration, false);
	}
}

void AP9Character::InteractReleased()
{
	bool bWasHolding = GetWorldTimerManager().IsTimerActive(InteractHoldTimer);
	GetWorldTimerManager().ClearTimer(InteractHoldTimer);
	if (bWasHolding)
	{
		//상점호출
	}
}

void AP9Character::InteractHoldSucceeded()
{
	if (CurrentOverlappingAltar != nullptr)
	{
		CurrentOverlappingAltar->InteractionTimerComplete();
	}
}

void AP9Character::RotateMeshToTarget(AActor* InTargetActor)
{
	if (!InTargetActor || !WeaponMesh_rl) return;

	FVector TargetLocation = InTargetActor->GetActorLocation();
	FVector MyLocation = WeaponMesh_rl->GetComponentLocation();

	FVector Direction = TargetLocation - MyLocation;
	Direction.Z = 0.0f; // 수평 회전만 고려
	Direction.Normalize();

	FRotator TargetRotation = Direction.Rotation();
	WeaponMesh_rl->SetWorldRotation(TargetRotation);
}