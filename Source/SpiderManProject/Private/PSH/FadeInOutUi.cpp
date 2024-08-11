// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/FadeInOutUi.h"
#include "Components/WidgetSwitcher.h"

void UFadeInOutUi::NativeConstruct()
{
	Super::NativeConstruct();

	WidgetSwitcher->SetActiveWidgetIndex(0);
}


void UFadeInOutUi::OnAnimStart()
{
	PlayAnimationForward(pageFade);
}

void UFadeInOutUi::OnMyRestart()
{

}

