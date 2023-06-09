// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HSCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class AHSWeapon;
class UHSHealthComponent;

UCLASS()
class COOPHORDESHOOTER_API AHSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHSCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void BeginCrouch();

	void EndCrouch();

	void BeginJump();

	void EndJump();

	void BeginZoom();

	void EndZoom();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHSHealthComponent* HealthComp;

	bool bIsZoomed;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float ZoomedFOV;

	float DefaultFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
	float ZoomInterpSpeed;

	UPROPERTY(Replicated)
	AHSWeapon* CurrentWeapon;

	float CurrentWeaponBulletSpread;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<AHSWeapon> StarterWeaponClass;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponAttachSocketName;

	UFUNCTION()
	void OnHealthChanged(UHSHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool bIsDead;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;
	
	UFUNCTION(BlueprintCallable, Category = "Player")
	void StartShooting();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StopShooting();

	const UHSHealthComponent* GetHealthComp() const;
};
