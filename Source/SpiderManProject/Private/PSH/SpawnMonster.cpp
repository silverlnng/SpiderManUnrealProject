// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/SpawnMonster.h"
#include "PSH/SpawnMonsterFSM.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ASpawnMonster::ASpawnMonster()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/SH/SpawnMonster/Mesh/SKM_SpawnMonster.SKM_SpawnMonster'"));

	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0,0,-90),FRotator(0,-90,0));
	}

	ConstructorHelpers::FClassFinder<UAnimInstance> animClass(TEXT("/Script/Engine.AnimBlueprint'/Game/SH/SpawnMonster/Anim/ABP_SpawnMonster.ABP_SpawnMonster_C'"));

	SwordCol = CreateDefaultSubobject<UCapsuleComponent>(TEXT("SwrodCol"));
	SwordCol->SetupAttachment(GetMesh(), TEXT("Weapon_R"));
	SwordCol->SetRelativeLocation(FVector(0, 12, 65));
	SwordCol->SetCollisionProfileName("NegativeWeapon");
	SwordCol->SetCapsuleHalfHeight(80);
	SwordCol->SetCapsuleRadius(10);


	Sword = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Sword"));
	Sword->SetupAttachment(SwordCol);
	Sword->SetRelativeLocation(FVector(0, 0, -70));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> SwrodMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/NiagaraMagicalSlashes/Model/Mesh/SK_Sword3.SK_Sword3'"));
	if (animClass.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(animClass.Class);
	}
	GetCharacterMovement()->bOrientRotationToMovement = true;
	FSM = CreateDefaultSubobject<USpawnMonsterFSM>(TEXT("FSM"));
}

// Called when the game starts or when spawned
void ASpawnMonster::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpawnMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASpawnMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ASpawnMonster::Setvisble(bool chek)
{
	Sword->SetVisibility(chek);
}

