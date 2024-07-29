// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/MisterNegative.h"
#include "PSH/MisterNegativeFSM.h"

// Sets default values
AMisterNegative::AMisterNegative()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh>tempMesh (TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequin_UE4/Meshes/SK_Mannequin.SK_Mannequin'")); 

	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0,0,-90),FRotator(0,-90,0));
	}

	MisterFSM = CreateDefaultSubobject<UMisterNegativeFSM>(TEXT("MisterFSM"));

	ConstructorHelpers::FClassFinder<UAnimInstance> animClass (TEXT("/Script/Engine.AnimBlueprint'/Game/SH/BluePrints/ABP_MisterNegative.ABP_MisterNegative_C'"));

	if (animClass.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(animClass.Class);
	}

	Sword = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Sword"));
	Sword->SetupAttachment(GetMesh(),TEXT("Weapon_R"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> SwrodMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/NiagaraMagicalSlashes/Model/Mesh/SK_Sword2.SK_Sword2'"));

	if (SwrodMesh.Succeeded())
	{
		Sword->SetSkeletalMesh(SwrodMesh.Object);
	}

}

// Called when the game starts or when spawned
void AMisterNegative::BeginPlay()
{
	Super::BeginPlay();
	
	

}

// Called every frame
void AMisterNegative::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMisterNegative::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMisterNegative::SpawnLightning()
{
	FActorSpawnParameters parm;
	parm.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GetWorld()->SpawnActor<AActor>(Lightning,GetActorLocation(),GetActorRotation(),parm);
}

void AMisterNegative::SpawnSpin()
{
	FTransform f = GetActorTransform();
	FActorSpawnParameters parm;
	parm.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GetWorld()->SpawnActor<AActor>(Spin, GetActorLocation(), GetActorRotation(), parm);
}

void AMisterNegative::SpawnCharging()
{
	FTransform f = GetActorTransform();
	FActorSpawnParameters parm;
	parm.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GetWorld()->SpawnActor<AActor>(Charging, GetActorLocation(), GetActorRotation(), parm);
}

