// Fill out your copyright notice in the Description page of Project Settings.


#include "YJ/Cable.h"

#include "CableComponent.h"

// Sets default values
ACable::ACable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));

	RootComponent = SceneComp;

	CableComp =CreateDefaultSubobject<UCableComponent>(TEXT("CableComponent"));

	CableComp->SetupAttachment(SceneComp);

}

// Called when the game starts or when spawned
void ACable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

