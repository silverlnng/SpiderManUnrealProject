// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NagetiveGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class SPIDERMANPROJECT_API ANagetiveGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;


public:

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> FadeInoutUiFac;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> EndUiFac;

	UPROPERTY(EditAnywhere)
	class UFadeInOutUi* FadeInoutUi;
	UPROPERTY(EditAnywhere)
	class UMisterNegativeUI* EndUi;

	void OnFadeIn();
	void SetNegativeUI();
	void SetFadeInOutUI();

	void DestroyFadeInOutUi();
};
