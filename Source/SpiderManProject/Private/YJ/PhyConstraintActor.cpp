// Fill out your copyright notice in the Description page of Project Settings.


#include "YJ/PhyConstraintActor.h"

#include "PhysicsEngine/PhysicsConstraintComponent.h"

// Sets default values
APhyConstraintActor::APhyConstraintActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));

	RootComponent = SceneComp;
	
	PhysicsConstraintComponent = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("PhysicsConstraintComponent"));
	PhysicsConstraintComponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void APhyConstraintActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APhyConstraintActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

