// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HSCrosshairWidget.generated.h"

/**
 *
 */
UCLASS()
class COOPHORDESHOOTER_API UHSCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()


protected:

	virtual void NativeConstruct() override;

public:

	UFUNCTION(BlueprintImplementableEvent, Category = "Animation")
	void PlayWidgetAnimation();
};
