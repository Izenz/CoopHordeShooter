// Fill out your copyright notice in the Description page of Project Settings.

#include "HSCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"

// Sets default values
AHSCharacter::AHSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanJump = true;
	JumpMaxHoldTime = 0.2f;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
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

void AHSCharacter::BeginCrouch()
{
	Crouch();
}

void AHSCharacter::EndCrouch()
{
	UnCrouch();
}

void AHSCharacter::BeginJump()
{
	Jump();
}

void AHSCharacter::EndJump()
{
	StopJumping();
}

// Called every frame
void AHSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bWasJumping)
	{
		JumpKeyHoldTime += DeltaTime;
	}
}

// Called to bind functionality to input
void AHSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AHSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHSCharacter::MoveRight);
	
	PlayerInputComponent->BindAxis("LookUp", this, &AHSCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &AHSCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AHSCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AHSCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AHSCharacter::BeginJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AHSCharacter::EndJump);

}

