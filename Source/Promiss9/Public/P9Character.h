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
	// 기본 및 앞구르기 속도
	float NormalSpeed;
	float SpeedMultiplier;
	float ForwardRollSpeed;

	// 앞구르기 여부
	bool bForwardRolling;

	// 앞구르기 속도 계산
	void UpdateRollingSpeed();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	AP9Character();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* OverheadWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
	TSubclassOf<UUserWidget> BuffInfoWidgetClass;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Amount);

protected:
	//Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	// Life cycle
	virtual void BeginPlay() override;
	//virtual void TakeDamage(
	//	float DamageAmount,
	//	struct FDamageEvent const& DamageEvent,
	//	AController* EventInstigator,
	//	AActor* DamageCauser) override;

	void OnDeath();
	void UpdateOverheadHP();

	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void StartJump(const FInputActionValue& value);
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void StartForwardRoll(const FInputActionValue& value);
	UFUNCTION()
	void StopForwardRoll(const FInputActionValue& value);
	UFUNCTION()
	void Interact(const FInputActionValue& value);
};
