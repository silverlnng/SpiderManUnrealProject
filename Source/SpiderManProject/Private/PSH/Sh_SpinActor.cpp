// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/Sh_SpinActor.h"
#include "Components/BoxComponent.h"
#include "YJ/SpiderMan.h"

// Sets default values
ASh_SpinActor::ASh_SpinActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	col = CreateDefaultSubobject<UBoxComponent>(TEXT("Col"));
	SetRootComponent(col);
	col->SetBoxExtent(FVector(25,230,20));
	col->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	mesh->SetupAttachment(RootComponent);
	mesh->SetRelativeLocation(FVector(550.0,0,0));
	mesh->SetRelativeRotation(FRotator(0,-90,0));
	mesh->SetRelativeScale3D(FVector(1.5f, 18.75f, 3));
	mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	col->SetCollisionProfileName(TEXT("NegativeWeapon"));

	ConstructorHelpers::FObjectFinder<UStaticMesh>tempMesh(TEXT("/Script/Engine.StaticMesh'/Game/NiagaraMagicalSlashes/Model/SM_Slash_01.SM_Slash_01'"));

	if (tempMesh.Succeeded())
	{
		mesh->SetStaticMesh(tempMesh.Object);
	}

}

// Called when the game starts or when spawned
void ASh_SpinActor::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(3);
	col->OnComponentBeginOverlap.AddDynamic(this, &ASh_SpinActor::OnComponentBeginOverlap);
	
}

// Called every frame
void ASh_SpinActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorLocation(GetActorLocation()+GetActorForwardVector()*speed*DeltaTime);
}

void ASh_SpinActor::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	ASpiderMan* player = Cast<ASpiderMan>(OtherActor);

	if (player != nullptr)
	{
		player->LaunchCharacter(GetActorForwardVector()*1000, false, false);
		player->Damaged(1);
		Destroy();
	}
}

