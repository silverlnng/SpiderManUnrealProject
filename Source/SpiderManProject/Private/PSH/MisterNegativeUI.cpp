// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/MisterNegativeUI.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UMisterNegativeUI::NativeConstruct()
{
	Super::NativeConstruct();

	MainButton->OnClicked.AddDynamic(this, &UMisterNegativeUI::OnMainWidget);
	RePlayButton->OnClicked.AddDynamic(this, &UMisterNegativeUI::OnRePlayWidget);
}

void UMisterNegativeUI::PlayerAnim()
{
	PlayAnimationForward(MissionClear);
}

void UMisterNegativeUI::OnMainWidget()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName("Start_Level"));
}

void UMisterNegativeUI::OnRePlayWidget()
{
	auto* pc = GetWorld()->GetFirstPlayerController();
	pc->SetShowMouseCursor(false);
	pc->SetInputMode(FInputModeGameOnly());
	UGameplayStatics::OpenLevel(GetWorld(), FName("SpiderPhase1"));
}
