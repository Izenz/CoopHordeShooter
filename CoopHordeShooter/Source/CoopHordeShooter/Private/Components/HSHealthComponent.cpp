// Fill out your copyright notice in the Description page of Project Settings.

#include "HSHealthComponent.h"
#include "HSGameMode.h"
#include "HSPlayerController.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
UHSHealthComponent::UHSHealthComponent()
{
	MaxHealth = 100;
	Heal(MaxHealth);
	bIsDead = false;
	SetIsReplicated(true);

	TeamId = 255;
}


// Called when the game starts
void UHSHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Only hook if we are server
	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UHSHealthComponent::HandleTakeAnyDamage);
		}
	}
	
	Health = MaxHealth;
}

void UHSHealthComponent::OnRep_Health(float OldHealth)
{
	float Damage = Health - OldHealth;

	OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}

void UHSHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || bIsDead)	return;

	if (DamageCauser != DamagedActor && IsFriendly(DamagedActor, DamageCauser))
	{
		return;
	}

	Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health));

	bIsDead = Health <= 0.0f;

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

	if (bIsDead)
	{
		AHSGameMode* GM = Cast<AHSGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
		}
	}
}

float UHSHealthComponent::GetHealth() const
{
	return Health;
}

bool UHSHealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB)
{
	if (ActorA == nullptr || ActorB == nullptr)
	{
		return true;
	}

	UHSHealthComponent* HealthCompA = Cast<UHSHealthComponent>(ActorA->GetComponentByClass(UHSHealthComponent::StaticClass()));
	UHSHealthComponent* HealthCompB = Cast<UHSHealthComponent>(ActorB->GetComponentByClass(UHSHealthComponent::StaticClass()));

	if (HealthCompA == nullptr || HealthCompB == nullptr)
	{
		return true;
	}

	return HealthCompA->TeamId == HealthCompB->TeamId;
}

bool UHSHealthComponent::IsDead(AActor* OtherActor)
{
	if (OtherActor == nullptr)	return true;

	UHSHealthComponent* HealthComp = Cast<UHSHealthComponent>(OtherActor->GetComponentByClass(UHSHealthComponent::StaticClass()));
	if (HealthComp == nullptr)	return true;

	return HealthComp->Health <= 0.0f;
}

void UHSHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f || Health <= 0.0f)	return;

	Health = FMath::Clamp(Health + HealAmount, 0.0f, MaxHealth);
	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s (+%s)"), *FString::SanitizeFloat(Health), *FString::SanitizeFloat(HealAmount));

	OnHealthChanged.Broadcast(this, Health, -HealAmount, nullptr, nullptr, nullptr);
}

void UHSHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHSHealthComponent, Health);
}

