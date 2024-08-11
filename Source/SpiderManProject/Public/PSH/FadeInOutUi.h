// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FadeInOutUi.generated.h"

/**
 * 
 */
UCLASS()
class SPIDERMANPROJECT_API UFadeInOutUi : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (BindWidgetAnim),Transient,Category = StandBy)
	class UWidgetAnimation * pageFade;

	UPROPERTY(meta=(BindWidget))
	class UWidgetSwitcher * WidgetSwitcher;
	UFUNCTION()
	void OnAnimStart();

	UFUNCTION()
	void OnMyRestart();

	virtual void NativeConstruct() override;
};
