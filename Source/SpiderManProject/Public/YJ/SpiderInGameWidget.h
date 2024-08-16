// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpiderInGameWidget.generated.h"

/**
 * 
 */
UCLASS()
class SPIDERMANPROJECT_API USpiderInGameWidget : public UUserWidget
{
	GENERATED_BODY()

private:	
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* PB_HP;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_HP;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_ComboNum;

	UFUNCTION()
	void SetHealthBar(float val);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float hp_Origin;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float hp_before;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float hp_After;
	
};
