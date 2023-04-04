// Fill out your copyright notice in the Description page of Project Settings.

#include "HSTrackerBall.h"
#include "HSCharacter.h"
#include "Components/HSHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationPath.h"
#include "DrawDebugHelpers.h"
#include "Sound/SoundCue.h"

// Sets default values
AHSTrackerBall::AHSTrackerBall()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(200.0f);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetupAttachment(RootComponent);

	HealthComp = CreateDefaultSubobject<UHSHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &AHSTrackerBall::HandleTakeAnyDamage);

	bUseVelocityChange = false;
	bHasExploded = false;
	bStartedSelfDestruction = false;
	MovementSpeed = 1000.0f;
	RequiredDistanceToTarget = 100.0f;
	ExplosionRadius = 200.0f;
	ExplosionDamage = 50.0f;
	SelfDamageInterval = 0.3f;
}

// Called when the game starts or when spawned
void AHSTrackerBall::BeginPlay()
{
	Super::BeginPlay();

	NextPathPoint = GetNextPathPoint();
}

void AHSTrackerBall::HandleTakeAnyDamage(UHSHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}

	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}

	if (Health <= 0.0f)
	{
		SelfDestruct();
	}

	UE_LOG(LogTemp, Log, TEXT("Health %s of %s"), *FString::SanitizeFloat(Health), *GetName());
}

FVector AHSTrackerBall::GetNextPathPoint()
{
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);
	UNavigationPath* NavPath = UNavigationSystem::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

	if (NavPath->PathPoints.Num() > 1)
	{
		// Return next point in path
		return NavPath->PathPoints[1];
	}

	// Failed to find path
	return GetActorLocation();
}

void AHSTrackerBall::SelfDestruct()
{
	if (bHasExploded)
	{
		return;
	}

	bHasExploded = true;
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionVFX, GetActorLocation());

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);

	UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);
	DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Yellow, false, 3.0f, 0, 1.0f);

	UGameplayStatics::PlaySoundAtLocation(this, ExplosionSFX, GetActorLocation());

	Destroy();
}

void AHSTrackerBall::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
}

// Called every frame
void AHSTrackerBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

	if (DistanceToTarget <= RequiredDistanceToTarget)
	{
		NextPathPoint = GetNextPathPoint();
	}
	else
	{
		FVector MovementDirection = NextPathPoint - GetActorLocation();
		MovementDirection.Normalize();

		MovementDirection *= MovementSpeed;

		MeshComp->AddForce(MovementDirection, NAME_None, bUseVelocityChange);

		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + MovementDirection, 32, FColor::Red, false, 0.0f, 0, 1.0f);
	}

	DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Red, false, 4.0f, 1.0f);
}

void AHSTrackerBall::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (bStartedSelfDestruction) return;

	AHSCharacter* PlayerPawn = Cast<AHSCharacter>(OtherActor);

	if (PlayerPawn)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &AHSTrackerBall::DamageSelf, SelfDamageInterval, true, 0.0f);
		bStartedSelfDestruction = true;

		UGameplayStatics::SpawnSoundAttached(SelfDestructSFX, RootComponent);
	}
}
