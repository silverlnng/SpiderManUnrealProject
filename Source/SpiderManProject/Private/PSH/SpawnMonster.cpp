// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/SpawnMonster.h"
#include "PSH/SpawnMonsterFSM.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "YJ/SpiderMan.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"
#include "PSH/MisterNegative.h"
#include "Kismet/GameplayStatics.h"
#include "PSH/MisterNegativeFSM.h"

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

	RightCol = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RightCol"));
	RightCol->SetupAttachment(GetMesh(), TEXT("Weapon_R"));
	RightCol->SetRelativeLocation(FVector(0, 5, -3));
	RightCol->SetCollisionProfileName("NegativeWeapon");
	RightCol->SetCapsuleHalfHeight(10);
	RightCol->SetCapsuleRadius(10);

	LeftCol = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LeftCol"));
	LeftCol->SetupAttachment(GetMesh(), TEXT("Weapon_L"));
	LeftCol->SetRelativeLocation(FVector(0, 5, -3));
	LeftCol->SetCollisionProfileName("NegativeWeapon");
	LeftCol->SetCapsuleHalfHeight(10);
	LeftCol->SetCapsuleRadius(10);

	FSM = CreateDefaultSubobject<USpawnMonsterFSM>(TEXT("FSM"));

	Naiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	Naiagara->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASpawnMonster::BeginPlay()
{
	Super::BeginPlay();
	EndAttack();

	MisterNegative = Cast<AMisterNegative>(UGameplayStatics::GetActorOfClass(GetWorld(),AMisterNegative::StaticClass()));

	MisterNegative->MisterFSM->SetMonster(this);
	
	LeftCol->OnComponentBeginOverlap.AddDynamic(this, &ASpawnMonster::LeftComponentBeginOverlap);

	RightCol->OnComponentBeginOverlap.AddDynamic(this, &ASpawnMonster::RightComponentBeginOverlap);

	SwordCol->OnComponentBeginOverlap.AddDynamic(this, &ASpawnMonster::SwordComponentBeginOverlap);
}

// Called every frame
void ASpawnMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bisDissolve) // 사라지게
	{
		dissolveAnimValue += DeltaTime / 4;
		GetMesh()->SetScalarParameterValueOnMaterials(TEXT("Animation"), dissolveAnimValue);
		Sword->SetScalarParameterValueOnMaterials(TEXT("Animation"), dissolveAnimValue);

		if (dissolveAnimValue > 1)
		{
			bisDissolve = false;
		}
	}

	if (SetisDissolve) // 나타나게
	{
		dissolveAnimValue -= DeltaTime;
		GetMesh()->SetScalarParameterValueOnMaterials(TEXT("Animation"), dissolveAnimValue);
		Sword->SetScalarParameterValueOnMaterials(TEXT("Animation"), dissolveAnimValue);
		if (dissolveAnimValue < 0)
		{
			Naiagara->SetVisibility(false);
			SetisDissolve = false;
		}
	}
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

void ASpawnMonster::SpawnLightning()
{
	FActorSpawnParameters parm;
	parm.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GetWorld()->SpawnActor<AActor>(Lightning, GetActorLocation(), GetActorRotation(), parm);
}

void ASpawnMonster::SwordAttack()
{
	SwordCol->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ASpawnMonster::RightAttack()
{
	RightCol->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ASpawnMonster::LeftAttack()
{
	LeftCol->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ASpawnMonster::EndAttack()
{
	SwordCol->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightCol->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftCol->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASpawnMonster::SetDissolveAnim()
{
	bisDissolve = true;
}

void ASpawnMonster::SetDissolveInit()
{
	
	SetisDissolve = true;
}


void ASpawnMonster::StopEffect()
{

}

void ASpawnMonster::SwordComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASpiderMan* player = Cast<ASpiderMan>(OtherActor);

	if (player != nullptr)
	{
		EndAttack();
		player->LaunchCharacter(GetActorForwardVector() * 1000, false, false);
		player->Damaged(1);
	}
}

void ASpawnMonster::LeftComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASpiderMan* player = Cast<ASpiderMan>(OtherActor);

	if (player != nullptr)
	{
		EndAttack();
		player->LaunchCharacter(GetActorForwardVector() * 1000, false, false);
		player->Damaged(1);
	}
}

void ASpawnMonster::RightComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASpiderMan* player = Cast<ASpiderMan>(OtherActor);

	if (player != nullptr)
	{
		EndAttack();
		player->LaunchCharacter(GetActorForwardVector() * 1000, false, false);
		player->Damaged(1);
	}
}

