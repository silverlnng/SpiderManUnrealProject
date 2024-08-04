// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/MisterNegative.h"
#include "PSH/MisterNegativeFSM.h"
#include "Components/CapsuleComponent.h"
#include "YJ/SpiderMan.h"

// Sets default values
AMisterNegative::AMisterNegative()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ConstructorHelpers::FObjectFinder<USkeletalMesh>tempMesh (TEXT("/Script/Engine.SkeletalMesh'/Game/SH/Asset/Mister_Negative/Negative.Negative'")); 

	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0,0,-90),FRotator(0,-90,0));
	}

	MisterFSM = CreateDefaultSubobject<UMisterNegativeFSM>(TEXT("MisterFSM"));

	ConstructorHelpers::FClassFinder<UAnimInstance> animClass (TEXT("/Script/Engine.AnimBlueprint'/Game/SH/BluePrints/ABP_Negative.ABP_Negative'"));

	if (animClass.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(animClass.Class);
	}

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Negative"));
	SwordCol = CreateDefaultSubobject<UCapsuleComponent>(TEXT("SwrodCol"));
	SwordCol->SetupAttachment(GetMesh(), TEXT("Weapon_R"));
	SwordCol->SetRelativeLocation(FVector(-6,12,65));
	SwordCol->SetCollisionProfileName("NegativeWeapon");
	SwordCol->SetCapsuleHalfHeight(80);
	SwordCol->SetCapsuleRadius(10);

	Sword = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Sword"));
	Sword->SetupAttachment(SwordCol);
	Sword->SetRelativeLocation(FVector(0, 0, -70));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> SwrodMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/NiagaraMagicalSlashes/Model/Mesh/SK_Sword2.SK_Sword2'"));

	if (SwrodMesh.Succeeded())
	{
		Sword->SetSkeletalMesh(SwrodMesh.Object);
	}

	Demon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DemonMesh"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh>tempDemonMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/SH/Asset/saw-demon/saw-demon.saw-demon'"));

	if (tempDemonMesh.Succeeded())
	{
		Demon->SetSkeletalMesh(tempDemonMesh.Object);
		Demon->SetupAttachment(GetMesh());
		Demon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Demon->SetRelativeLocation(FVector(-1, -1.9f, -7.6f));
		Demon->SetRelativeScale3D(FVector(3,3,2.5));
	}

	ConstructorHelpers::FClassFinder<UAnimInstance> DemonAnimClass(TEXT("/Script/Engine.AnimBlueprint'/Game/SH/BluePrints/ABP_Demon.ABP_Demon_C'"));

	if (DemonAnimClass.Succeeded())
	{
		Demon->SetAnimInstanceClass(DemonAnimClass.Class);
	}

	demonCol = CreateDefaultSubobject<UCapsuleComponent>(TEXT("demonCol"));
	demonCol->SetupAttachment(Demon,TEXT("arm-right-elbow"));
	demonCol->SetRelativeLocationAndRotation(FVector(-0.4f,0.6,0.45f),FRotator(15,25,63.5));
	demonCol->SetCollisionProfileName("NegativeWeapon");
	demonCol->SetCapsuleRadius(0.45);
	demonCol->SetCapsuleHalfHeight(3);
	
}

// Called when the game starts or when spawned
void AMisterNegative::BeginPlay()
{
	Super::BeginPlay();
	
	Demon->SetVisibility(false);
	demonCol->OnComponentBeginOverlap.AddDynamic(this,&AMisterNegative::DemonComponentBeginOverlap);

	SwordCol->OnComponentBeginOverlap.AddDynamic(this,&AMisterNegative::SwordComponentBeginOverlap);
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

void AMisterNegative::SetMeshVisible(bool chek)
{
	Sword->SetVisibility(!chek);
	Demon->SetVisibility(chek);
}

void AMisterNegative::CameraShake()
{
	GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(Cs_DemonAttack, 0.5f);
}

void AMisterNegative::SwordComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASpiderMan* player = Cast<ASpiderMan>(OtherActor);

	if (player != nullptr)
	{
		SwordCol->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		player->LaunchCharacter(GetActorForwardVector() * 1000, false, false);
		player->Damaged(1);
	}
}

void AMisterNegative::DemonComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASpiderMan* player = Cast<ASpiderMan>(OtherActor);

	if (player != nullptr)
	{
		demonCol->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		player->LaunchCharacter(GetActorForwardVector() * 1000, false, false);
		player->Damaged(1);
	}
}

