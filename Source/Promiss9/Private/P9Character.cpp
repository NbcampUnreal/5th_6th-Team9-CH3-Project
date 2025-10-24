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

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	//UI
	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(GetMesh());
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);

	NormalSpeed = 600.0f;
	SpeedMultiplier = 1.0f;
	ForwardRollSpeed = 900.0f;

	MaxHealth = 100.0f;
	Health = MaxHealth;
}

void AP9Character::BeginPlay()
{
	Super::BeginPlay();
	UpdateOverheadHP();
	
}

// Called every frame
void AP9Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

void AP9Character::Move(const FInputActionValue& value)
{
	if (!Controller)
	{
		return;
	}

	FVector2D MoveInput = value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		float RotationSpeed = 90.0f; // 초당 회전 속도 (도 단위)
		float DeltaTime = GetWorld()->GetDeltaSeconds();
		float YawDelta = MoveInput.Y * RotationSpeed * DeltaTime;

		FRotator NewRotation = GetActorRotation();
		NewRotation.Yaw += YawDelta;

		SetActorRotation(NewRotation);
	}
}

void AP9Character::StartJump(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		Jump();
	}
}

void AP9Character::StopJump(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		StopJumping();
	}
}

void AP9Character::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();
	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void AP9Character::StartForwardRoll(const FInputActionValue& value)
{
	bForwardRolling = true;
	UpdateRollingSpeed();
}

void AP9Character::StopForwardRoll(const FInputActionValue& value)
{
	bForwardRolling = false;
	UpdateRollingSpeed();
}

void AP9Character::Interact(const FInputActionValue& value)
{

}

float AP9Character::GetHealth() const
{
	return Health;
}

void AP9Character::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
	UpdateOverheadHP();
}

void AP9Character::OnDeath()
{

}

void AP9Character::UpdateOverheadHP()
{
	if (UUserWidget* WidgetInstance = OverheadWidget->GetUserWidgetObject())
	{
		if (UProgressBar* HPBar = Cast<UProgressBar>(WidgetInstance->GetWidgetFromName(TEXT("HealthBar"))))
		{
			const float HPPercent = (MaxHealth > 0.f) ? Health / MaxHealth : 0.f;
			HPBar->SetPercent(HPPercent);

			if (HPPercent < 0.3f)
			{
				HPBar->SetFillColorAndOpacity(FLinearColor::Red);
			}
		}

		if (UTextBlock* HPText = Cast<UTextBlock>(WidgetInstance->GetWidgetFromName(TEXT("HPText"))))
		{
			HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));
		}
	}
}

void AP9Character::UpdateRollingSpeed()
{
	const float BaseSpeed = bForwardRolling ? ForwardRollSpeed : NormalSpeed;
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed * SpeedMultiplier;
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