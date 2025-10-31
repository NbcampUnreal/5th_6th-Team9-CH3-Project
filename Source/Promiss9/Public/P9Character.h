// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "P9InventoryComponent.h"
#include "P9Character.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USphereComponent;
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

	// Inventory Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UP9InventoryComponent* InventoryComponent;

	// Health
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetHealth(float NewHealth);
	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Amount);

	// MaxHealth
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetMaxHealth(float NewMaxHealth);
	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddMaxHealth(float Amount);

	// 이동 속도
	UFUNCTION(BlueprintCallable, Category = "Speed")
	float GetNormalSpeed() const;
	UFUNCTION(BlueprintCallable, Category = "Speed")
	void SetNormalSpeed(float NewNormalSpeed);
	UFUNCTION(BlueprintCallable, Category = "Speed")
	void AddNormalSpeed(float Amount);

	//void UpdateMoveSpeed();

	// 앞구르기 관련
	void ResetRollCooldown();

	// 사망
	void OnDeath();

	// Damage
	UFUNCTION(BlueprintCallable)
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;

	UFUNCTION()
	void Move(const FInputActionValue& Value);
	UFUNCTION()
	void MoveCompleted(const FInputActionValue& Value);
	UFUNCTION()
	void StartJump(const FInputActionValue& Value);
	UFUNCTION()
	void StopJump(const FInputActionValue& Value);
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

	// 자유 시점
	void OnFreeLookStart(const FInputActionValue& Value);
	void OnFreeLookEnd(const FInputActionValue& Value);

	// 무기 장착
	void EquipWeaponToMultipleSockets();
	void EquipWeaponToRightHandSockets();

	// 라인트레이스 가장 가까운 적 추적
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	void RotateMeshToTarget(AActor* TargetActor);

	// 몽타주 실행 중 무기 숨기기
	void HideAllWeapons(bool bHide);

protected:

	// 무기 장착 관련 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Socket")
	USceneComponent* WaeponSocket_fr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Socket")
	USceneComponent* WaeponSocket_fl;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Socket")
	USceneComponent* WaeponSocket_rr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Socket")
	USceneComponent* WaeponSocket_rl;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Mesh")
	UStaticMeshComponent* WeaponMesh_fr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Mesh")
	UStaticMeshComponent* WeaponMesh_fl;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Mesh")
	UStaticMeshComponent* WeaponMesh_rr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Mesh")
	UStaticMeshComponent* WeaponMesh_rl;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	class USphereComponent* SphereCollision;

	// 앞구르기 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Roll")
	bool bForwardRolling;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Roll")
	bool bCanRoll;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Roll|Animation")
	UAnimMontage* ForwardRollMontage;

	// 기본 속도 및 앞구르기 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Speed")
	float NormalSpeed;
	float RollDistance;

	// 캐릭터 이동 관련
	float DefaultYaw;
	float TargetYawOffset; // 회전 오프셋
	float RotationInterpSpeed; // 회전 보간 속도
	bool bIsSideMoving; // 좌우 이동 여부
	bool bIsDiagonalMoving; // 대각선 이동 여부

	// 자유시점 관련
	float SavedArmLength;
	FRotator SavedControlRotation;
	bool bIsFreeLookMode;

	// Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	int32 MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	int32 Health;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health|Death")
	bool bIsDead = false;
	UPROPERTY(EditDefaultsOnly, Category = "Health|Death")
	UAnimMontage* DeathMontage;

	// Status
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	int32 CharacterLevel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float CurrentExp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float ExpToNextLevel;

	// Weapon 장착 관련
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<AActor> HandWeaponClass;
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<AActor> MultiWeaponClass;
	UPROPERTY()
	TArray<AActor*> EquippedWeapons;

	// 무기 적 추적 관련
	UPROPERTY(BlueprintReadWrite, Category = "Targeting")
	AActor* TargetActor;

	FOnMontageEnded RollMontageEndedDelegate;
	FTimerHandle RollCooldownTimerHandle;
};