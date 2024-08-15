// Fill out your copyright notice in the Description page of Project Settings.


#include "YJ/SpiderInGameWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void USpiderInGameWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void USpiderInGameWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 여기서 lerp를 이용해서 PB_HP 조절할꺼
		// before값이 점점 lerp으로 after 으로

	// 값이다를때까지 
	
	

	if(hp_before!=hp_After)
	{
		hp_before =FMath::Lerp(hp_before,hp_After,InDeltaTime*2);
		SetHealthBar(hp_before);
		int32 IntNumb = hp_before;
		FString str =  FString::Printf(TEXT("%d"), IntNumb);
		Text_HP->SetText(FText::FromString(str));
	}
	
}

void USpiderInGameWidget::SetHealthBar(float val)
{
	float rate = val/hp_Origin;
	PB_HP->SetPercent(rate);
	// 근데 lerp 하게
	// 새롭게 받은 val 과 현재 hp를 
}
