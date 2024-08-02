// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/Sh_ChargingActor.h"
#include "Components/BoxComponent.h"
#include "YJ/SpiderMan.h"
#include "Components/PrimitiveComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ASh_ChargingActor::ASh_ChargingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	col = CreateDefaultSubobject<UBoxComponent>(TEXT("Col"));
	SetRootComponent(col);
	col->SetBoxExtent(FVector(30, 300
	, 40));
	col->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	mesh->SetupAttachment(RootComponent);
	mesh->SetRelativeLocation(FVector(-460.0, 0, 0));
	mesh->SetRelativeRotation(FRotator(0,-90,0));
	mesh->SetRelativeScale3D(FVector(2,2,1));
	mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ConstructorHelpers::FObjectFinder<UStaticMesh>tempMesh(TEXT("/Script/Engine.StaticMesh'/Game/NiagaraMagicalSlashes/Model/SM_Slash_02.SM_Slash_02'"));
	col->SetCollisionProfileName(TEXT("NegativeWeapon"));
	if (tempMesh.Succeeded())
	{
		mesh->SetStaticMesh(tempMesh.Object);
	}
}

// Called when the game starts or when spawned
void ASh_ChargingActor::BeginPlay()
{
	Super::BeginPlay();
	
	SetLifeSpan(3);
	col->OnComponentBeginOverlap.AddDynamic(this,&ASh_ChargingActor::OnComponentBeginOverlap);
}

// Called every frame
void ASh_ChargingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorLocation(GetActorLocation() + GetActorForwardVector() * speed * DeltaTime);
}

void ASh_ChargingActor::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASpiderMan * player = Cast<ASpiderMan>(OtherActor);

	if (player != nullptr)
	{
		player->LaunchCharacter(GetActorForwardVector() * 1000, false, false);
		Destroy();
		player->Damaged(1);
	}
}


