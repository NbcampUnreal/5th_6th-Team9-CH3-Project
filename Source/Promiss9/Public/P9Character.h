// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "P9Character.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
class UUserWidget;

struct FInputActionValue;

UCLASS()
class PROMISS9_API AP9Character : public ACharacter
{
	GENERATED_BODY()

public:
	AP9Character();

	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	// Health
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Amount);

	// Status
	UFUNCTION()
	int GetCharacterLevel() const;
	UFUNCTION()
	void AddExperience(int Amount);
	UFUNCTION()
	void LevelUp();
	UFUNCTION()
	void SetLevel(int NewLevel);

	// 이동 속도
	void UpdateMoveSpeed();

	// 앞구르기 관련
	void ResetRollCooldown();

	// 사망
	void OnDeath();



	// Life cycle
	//virtual void TakeDamage(
	//	float DamageAmount,
	//	struct FDamageEvent const& DamageEvent,
	//	AController* EventInstigator,
	//	AActor* DamageCauser) override;

	UFUNCTION()
	void Move(const FInputActionValue& Value);
	UFUNCTION()
	void TurnCharacter(const FInputActionValue& Value);
	UFUNCTION()
	void StartJump(const FInputActionValue& Value);
	UFUNCTION()
	void StopJump(const FInputActionValue& Value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& Value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& Value);
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	UFUNCTION()
	void ZoomCamera(const FInputActionValue& Value);
	UFUNCTION()
	void StartForwardRoll();
	UFUNCTION()
	void StopForwardRoll();
	UFUNCTION()
	void OnRollMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void Interact(const FInputActionValue& Value);

	//LMB 누르기/떼기 시
	void OnFreeLookStart(const FInputActionValue& Value);
	void OnFreeLookEnd(const FInputActionValue& Value);

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* OverheadWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	TSubclassOf<UUserWidget> BuffInfoWidgetClass;

	// 앞구르기 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Roll")
	bool bForwardRolling;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Roll")
	bool bCanRoll;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Roll|Animation")
	UAnimMontage* ForwardRollMontage;

	// 기본 및 앞구르기 속도
	float NormalSpeed;
	float SprintSpeedMultiplier;
	float SprintSpeed;
	float ForwardRollSpeed;

	// 캐릭터 회전 관련
	FRotator TargetRotation;
	float RotationInterpSpeed;
	bool bShouldRotate;

	// 자유시점 여부
	bool bIsFreeLookMode;

	// 달리기 여부
	bool bSprinting;

	// Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	int MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	int Health;

	// Status
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	int CharacterLevel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	int CurrentExp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	int ExpToNextLevel;

	FOnMontageEnded RollMontageEndedDelegate;
	FTimerHandle RollCooldownTimerHandle;
};
