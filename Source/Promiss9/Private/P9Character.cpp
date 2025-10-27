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

	bIsFreeLookMode = false;
	SavedArmLength = 0.0f;

	// 이동관련 변수, 함수 초기화
	NormalSpeed = 600.0f;
	SprintSpeedMultiplier = 1.5f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = false;
	ForwardRollSpeed = 900.0f;
	bForwardRolling = false;
	bCanRoll = true;
	bSprinting = false;
	// 좌우 이동
	DefaultYaw = 0.f;
	TargetYawOffset = 0.f;
	RotationInterpSpeed = 8.0f;
	bIsSideMoving = false;

	// Health
	MaxHealth = 100.0f;
	Health = MaxHealth;

	// Level 관련
	CharacterLevel = 1;
	CurrentExp = 0;
	ExpToNextLevel = 100;
}

void AP9Character::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AP9Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Controller) return;

	if (!bIsFreeLookMode)
	{
		// 컨트롤러(카메라) 방향 기준으로 몸 회전
		FRotator ControlRot = Controller->GetControlRotation();
		FRotator TargetRot = ControlRot;
		TargetRot.Pitch = 0.f;
		TargetRot.Roll = 0.f;
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

			// 캐릭터 회전 바인딩
			if (PlayerController->TurnAction)
			{
				EnhancedInput->BindAction(
					PlayerController->TurnAction,
					ETriggerEvent::Triggered,
					this,
					&AP9Character::TurnCharacter
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

			// Sprint
			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&AP9Character::StartSprint
				);

				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&AP9Character::StopSprint
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
					ETriggerEvent::Triggered,
					this,
					&AP9Character::StartForwardRoll
				);

				EnhancedInput->BindAction(
					PlayerController->ForwardRollAction,
					ETriggerEvent::Completed,
					this,
					&AP9Character::StopForwardRoll
				);
			}

			// Interact
			if (PlayerController->InteractAction)
			{
				EnhancedInput->BindAction(
					PlayerController->InteractAction,
					ETriggerEvent::Completed,
					this,
					&AP9Character::Interact
				);
			}
		}
	}
}

void AP9Character::Move(const FInputActionValue& Value)
{
	if (!Controller)
	{
		return;
	}

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

void AP9Character::TurnCharacter(const FInputActionValue& Value)
{
	if (!Controller)
	{
		return;
	}
}

void AP9Character::StartJump(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		Jump();
	}
}

void AP9Character::StopJump(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		StopJumping();
	}
}

void AP9Character::StartSprint(const FInputActionValue& Value)
{
	bSprinting = true;
	UpdateMoveSpeed();
}

void AP9Character::StopSprint(const FInputActionValue& Value)
{
	bSprinting = false;
	UpdateMoveSpeed();
}

void AP9Character::Look(const FInputActionValue& Value)
{
	FVector2D LookInput = Value.Get<FVector2D>();

	if (!Controller) return;

	if (bIsFreeLookMode)
	{
		// 프리룩 중엔 컨트롤러 회전만 변경
		AddControllerYawInput(LookInput.X);
		AddControllerPitchInput(LookInput.Y);
	}
	else
	{
		// 일반 모드에서는 카메라와 캐릭터가 함께 회전
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

	// 카메라를 freelook 모드 이전 마지막 위치로 복원
	//SpringArmComp->TargetArmLength = SavedArmLength;
	//SpringArmComp->SetRelativeRotation(SavedSpringArmRotation);

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	if (Controller)
	{
		Controller->SetControlRotation(SavedControlRotation);
	}

	UE_LOG(LogTemp, Warning, TEXT(">> FreeLook End"));
}


void AP9Character::StartForwardRoll()
{
	if (bForwardRolling || !bCanRoll) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ForwardRollMontage)
	{
		bForwardRolling = true;
		bCanRoll = false;
		//몽타주 재생
		AnimInstance->Montage_Play(ForwardRollMontage);

		RollMontageEndedDelegate.BindUObject(this, &AP9Character::OnRollMontageEnded);
		AnimInstance->Montage_SetEndDelegate(RollMontageEndedDelegate, ForwardRollMontage);

		GetWorldTimerManager().SetTimer(RollCooldownTimerHandle, this, &AP9Character::ResetRollCooldown, 3.0f, false);
	}
}

void AP9Character::OnRollMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 다른 몽타주와 섞이지 않게 확인
	if (Montage == ForwardRollMontage)
	{
		bForwardRolling = false;
	}
}

void AP9Character::StopForwardRoll()
{
	bForwardRolling = false;
}

void AP9Character::Interact(const FInputActionValue& Value)
{
	{
		// 트레이스(시야 방향으로 레이저)
		FVector Start = GetActorLocation();
		FVector End = Start + GetActorForwardVector() * 300.0f;

		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this); // 자기 자신 무시

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

		FColor LineColor = bHit ? FColor::Green : FColor::Red;
		DrawDebugLine(GetWorld(), Start, End, LineColor, false, 2.0f, 0, 2.0f);

		if (bHit && HitResult.GetActor())
		{
			//// 액터와 상호작용
			//IInteractableInterface* Interactable = Cast<IInteractableInterface>(HitResult.GetActor());
			//if (Interactable)
			//{
			//	Interactable->Execute_OnInteract(HitResult.GetActor(), this);
			//}
		}
	}
}

void AP9Character::ResetRollCooldown()
{
	bCanRoll = true;
}

float AP9Character::GetHealth() const
{
	return Health;
}

void AP9Character::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
}

void AP9Character::OnDeath()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;
	GetCharacterMovement()->DisableMovement();
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->SetIgnoreMoveInput(true);
		PC->SetIgnoreLookInput(true);
	}
	
	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}

	UE_LOG(LogTemp, Warning, TEXT("%s 사망"), *GetName());

}

void AP9Character::UpdateMoveSpeed()
{
	const float BaseSpeed = bSprinting ? SprintSpeed : NormalSpeed;
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed * SprintSpeedMultiplier;
}

int AP9Character::GetCharacterLevel() const
{
	return CharacterLevel;
}

void AP9Character::AddExperience(int Amount)
{
	CurrentExp += Amount;

	while (CurrentExp >= ExpToNextLevel)
	{
		CurrentExp -= ExpToNextLevel;
	}
}

void AP9Character::LevelUp()
{
	CharacterLevel++;
	ExpToNextLevel += 50;
}

void AP9Character::SetLevel(int NewLevel)
{
	CharacterLevel = NewLevel;
	CurrentExp = 0;
	ExpToNextLevel = 100 + (NewLevel - 1) * 50;
}

float AP9Character::TakeDamage(
	float DamageAmount,
	struct FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);


	if (Health <= 0.0f)
	{
		OnDeath();
	}

	return ActualDamage;
}