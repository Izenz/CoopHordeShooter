// Fill out your copyright notice in the Description page of Project Settings.

#include "HSCharacter.h"
#include "HSWeapon.h"
#include "Components/HSHealthComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "CoopHordeShooter.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AHSCharacter::AHSCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanJump = true;
	JumpMaxHoldTime = 0.2f;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	HealthComp = CreateDefaultSubobject<UHSHealthComponent>(TEXT("HealthComp"));

	ZoomedFOV = 65.0f;
	ZoomInterpSpeed = 20;

	WeaponAttachSocketName = "WeaponSocket";
	bIsDead = false;
}

// Called when the game starts or when spawned
void AHSCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = CameraComp->FieldOfView;
	HealthComp->OnHealthChanged.AddDynamic(this, &AHSCharacter::OnHealthChanged);

	if (Role == ROLE_Authority)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		CurrentWeapon = GetWorld()->SpawnActor<AHSWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponAttachSocketName);
		}
	}	
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
	if(CurrentWeapon)
	{
		CurrentWeaponBulletSpread = CurrentWeapon->GetBulletspread();
		CurrentWeapon->SetBulletSpread(0.0f);
	}
}

void AHSCharacter::EndCrouch()
{
	UnCrouch();
	if (CurrentWeapon)
	{
		CurrentWeapon->SetBulletSpread(CurrentWeaponBulletSpread);
	}
}

void AHSCharacter::BeginJump()
{
	Jump();
}

void AHSCharacter::EndJump()
{
	StopJumping();
}

void AHSCharacter::BeginZoom()
{
	bIsZoomed = true;
}

void AHSCharacter::EndZoom()
{
	bIsZoomed = false;
}

void AHSCharacter::StartShooting()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartShooting();
	}
}

void AHSCharacter::StopShooting()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopShooting();
	}
}

void AHSCharacter::OnHealthChanged(UHSHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bIsDead)
	{
		bIsDead = true;

		StopShooting();

		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		DetachFromControllerPendingDestroy();
		SetLifeSpan(5.0f);
		CurrentWeapon->SetLifeSpan(5.0f);
	}
}

// Called every frame
void AHSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = bIsZoomed ? ZoomedFOV : DefaultFOV;
	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);
	CameraComp->SetFieldOfView(NewFOV);

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

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &AHSCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &AHSCharacter::EndZoom);

	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &AHSCharacter::StartShooting);
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &AHSCharacter::StopShooting);

}

FVector AHSCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

void AHSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHSCharacter, CurrentWeapon);
	DOREPLIFETIME(AHSCharacter, bIsDead);
}
