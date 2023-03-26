// Fill out your copyright notice in the Description page of Project Settings.

#include "HSHealthComponent.h"


// Sets default values for this component's properties
UHSHealthComponent::UHSHealthComponent()
{
	MaxHealth = 100;
}


// Called when the game starts
void UHSHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &UHSHealthComponent::HandleTakeAnyDamage);
	}

	Health = MaxHealth;
}

void UHSHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage < 0.0f)	return;

	Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health));

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
}

