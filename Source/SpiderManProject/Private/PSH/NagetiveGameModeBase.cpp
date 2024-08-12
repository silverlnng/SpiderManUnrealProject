// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/NagetiveGameModeBase.h"
#include "PSH/FadeInOutUi.h"
#include "PSH/MisterNegativeUI.h"

void ANagetiveGameModeBase::BeginPlay()
{
	FadeInoutUi = Cast<UFadeInOutUi>(CreateWidget(GetWorld(), FadeInoutUiFac));
	EndUi = Cast<UMisterNegativeUI>(CreateWidget(GetWorld(), EndUiFac));
	
}

void ANagetiveGameModeBase::OnFadeIn()
{
	FadeInoutUi->AddToViewport(0);
	FadeInoutUi->OnAnimStart();
}

void ANagetiveGameModeBase::SetNegativeUI()
{
	EndUi->AddToViewport(1);
	auto* pc = GetWorld()->GetFirstPlayerController();
	pc->SetShowMouseCursor(true);
	pc->SetInputMode(FInputModeUIOnly());
}

void ANagetiveGameModeBase::SetFadeInOutUI()
{
	
	
	
}

void ANagetiveGameModeBase::DestroyFadeInOutUi()
{

}
