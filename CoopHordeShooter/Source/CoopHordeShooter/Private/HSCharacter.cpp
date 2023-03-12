// Fill out your copyright notice in the Description page of Project Settings.

#include "HSCharacter.h"


// Sets default values
AHSCharacter::AHSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHSCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void AHSCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

// Called every frame
void AHSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AHSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AHSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHSCharacter::MoveRight);

}

