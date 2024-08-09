// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/Sh_LightningActor.h"
#include "Components/BoxComponent.h"
#include "YJ/SpiderMan.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Classes/NiagaraSystem.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"

// Sets default values
ASh_LightningActor::ASh_LightningActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	col = CreateDefaultSubobject<UBoxComponent>(TEXT("Col"));
	SetRootComponent(col);
	col->SetBoxExtent(FVector(100, 20, 250));
	col->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	mesh->SetupAttachment(RootComponent);
	mesh->SetRelativeLocation(FVector(-430, 0, 0));
	mesh->SetRelativeRotation(FRotator(0, -90, -60));
	mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ConstructorHelpers::FObjectFinder<UStaticMesh>tempMesh (TEXT("/Script/Engine.StaticMesh'/Game/NiagaraMagicalSlashes/Model/SM_Slash_03.SM_Slash_03'"));

	if (tempMesh.Succeeded())
	{
		mesh->SetStaticMesh(tempMesh.Object);
	}

	col->SetCollisionProfileName(TEXT("NegativeWeapon"));

	Naiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	Naiagara->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASh_LightningActor::BeginPlay()
{
	Super::BeginPlay();
	col->OnComponentBeginOverlap.AddDynamic(this, &ASh_LightningActor::OnComponentBeginOverlap);
	SetLifeSpan(2);
}

// Called every frame
void ASh_LightningActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetActorLocation(GetActorLocation() + GetActorForwardVector() * speed * DeltaTime);
}

void ASh_LightningActor::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASpiderMan* player = Cast<ASpiderMan>(OtherActor);

	if (player != nullptr)
	{
		player->LaunchCharacter(GetActorForwardVector() * 1000, false, false);
		Destroy();
		player->Damaged(1);
	}
}

