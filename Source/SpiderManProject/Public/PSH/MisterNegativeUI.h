// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MisterNegativeUI.generated.h"

/**
 * 
 */
UCLASS()
class SPIDERMANPROJECT_API UMisterNegativeUI : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget))
	class UButton * MainButton;

	UPROPERTY(meta=(BindWidget))
	class UButton * RePlayButton;

	
	UFUNCTION()
	void OnMainWidget();
	UFUNCTION()
	void OnRePlayWidget();
	
};
