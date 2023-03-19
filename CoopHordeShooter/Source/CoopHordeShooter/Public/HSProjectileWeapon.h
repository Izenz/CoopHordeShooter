// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HSWeapon.h"
#include "HSProjectileWeapon.generated.h"

/**
 *
 */
UCLASS()
class COOPHORDESHOOTER_API AHSProjectileWeapon : public AHSWeapon
{

	GENERATED_BODY()

protected:

	virtual void Shoot() override;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileWeapon")
	TSubclassOf<AActor> ProjectileClass;
};
