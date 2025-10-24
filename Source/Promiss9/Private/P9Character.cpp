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

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = true;
	bUseControllerRotationRoll = false;
	bIsFreeLookMode = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	//UI
	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(GetMesh());
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);

	// 디동관련 변수, 함수 초기화
	NormalSpeed = 600.0f;
	SprintSpeedMultiplier = 1.5f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	ForwardRollSpeed = 900.0f;
	TargetRotation = FRotator(0.0f, 0.0f, 0.0f);
	RotationInterpSpeed = 5.0f;
	bShouldRotate = false;
	bForwardRolling = false;
	bCanRoll = true;

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

	// 캐릭터의 부드러운 회전을 위한 함수
	if (bShouldRotate)
	{
		FRotator CurrentRotation = GetActorRotation();
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationInterpSpeed);

		SetActorRotation(NewRotation);

		// 회전이 거의 완료되었는지 확인
		if (NewRotation.Equals(TargetRotation, 0.5f))
		{
			bShouldRotate = false;
		}
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
					ETriggerEvent::Triggered,
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

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void AP9Character::TurnCharacter(const FInputActionValue& Value)
{
	FVector2D TurnDirection = Value.Get<FVector2D>(); // Q: -1, E: +1

	if (!FMath::IsNearlyZero(TurnDirection.X))
	{
		TargetRotation = GetActorRotation();
		TargetRotation.Yaw += TurnDirection.X * 45.0f;
		bShouldRotate = true;
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
	if (Controller != nullptr)
	{
		AddControllerPitchInput(LookInput.Y);
		AddControllerYawInput(LookInput.X);
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
}

void AP9Character::OnFreeLookEnd(const FInputActionValue& Value)
{
	bIsFreeLookMode = false;
	bUseControllerRotationYaw = true;
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



//void AP9Character::TakeDamage(
//	float DamageAmount,
//	struct FDamageEvent const& DamageEvent,
//	AController* EventInstigator,
//	AActor* DamageCauser)
//{
//	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
//
//	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
//	UpdateOverheadHP();
//
//	if (Health <= 0.0f)
//	{
//		OnDeath();
//	}
//
//	return ActualDamage;
//}