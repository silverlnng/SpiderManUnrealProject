// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/MisterNegative.h"
#include "PSH/MisterNegativeFSM.h"
#include "Components/CapsuleComponent.h"
#include "YJ/SpiderMan.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"
#include "PSH/SpawnMonsterAnim.h"
#include "Kismet/GameplayStatics.h"
#include "PSH/MonsterSpawner.h"
#include "PSH/FadeInOutUi.h"
#include "PSH/NagetiveGameModeBase.h"
#include "Components/WidgetComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
AMisterNegative::AMisterNegative()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh>tempMesh (TEXT("/Script/Engine.SkeletalMesh'/Game/SH/Asset/Mister_Negative/Negative.Negative'")); 

	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0,0,-90),FRotator(0,-90,0));
	}

	MisterFSM = CreateDefaultSubobject<UMisterNegativeFSM>(TEXT("MisterFSM"));

	ConstructorHelpers::FClassFinder<UAnimInstance> animClass (TEXT("/Script/Engine.AnimBlueprint'/Game/SH/BluePrints/ABP_Negative.ABP_Negative_C'"));

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
		Demon->SetRelativeScale3D(FVector(4,4,2.5));
	}

	ConstructorHelpers::FClassFinder<UAnimInstance> DemonAnimClass(TEXT("/Script/Engine.AnimBlueprint'/Game/SH/BluePrints/ABP_Demon.ABP_Demon_C'"));

	if (DemonAnimClass.Succeeded())
	{
		Demon->SetAnimInstanceClass(DemonAnimClass.Class);
	}

	demonColR = CreateDefaultSubobject<UCapsuleComponent>(TEXT("demonRCol"));
	demonColR->SetupAttachment(Demon,TEXT("arm-right-elbow"));
	demonColR->SetRelativeLocationAndRotation(FVector(-0.4f,0.6,0.45f),FRotator(15,25,63.5));
	demonColR->SetCollisionProfileName("NegativeWeapon");
	demonColR->SetCapsuleRadius(0.45);
	demonColR->SetCapsuleHalfHeight(3);

	demonColL = CreateDefaultSubobject<UCapsuleComponent>(TEXT("demonLCol"));
	demonColL->SetupAttachment(Demon,TEXT("arm-left-elbow"));
	demonColL->SetRelativeLocationAndRotation(FVector(-0.2f,0.04f,0.2f),FRotator(0,0,63.5));
	demonColL->SetCollisionProfileName("NegativeWeapon");
	demonColL->SetCapsuleRadius(0.45);
	demonColL->SetCapsuleHalfHeight(3);

	Naiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	Naiagara->SetupAttachment(Sword);
	Naiagara->SetRelativeLocationAndRotation(FVector(0,0,30),FRotator(-90,0,0));

	SturnUi = CreateDefaultSubobject<UWidgetComponent>(TEXT("SturnUi"));
	SturnUi->SetupAttachment(RootComponent);
	SturnUi->SetRelativeLocation(FVector(0,0,150));
}

// Called when the game starts or when spawned
void AMisterNegative::BeginPlay()
{
	Super::BeginPlay();
	Naiagara->SetVisibility(false);

	SetUiVisble(false);

	Gmb = Cast<ANagetiveGameModeBase>(GetWorld()->GetAuthGameMode());
	/*Gmb->SetFadeInOutUI();*/

	SpawnMonster = Cast<AMonsterSpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), AMonsterSpawner::StaticClass()));

	demonColL->OnComponentBeginOverlap.AddDynamic(this,&AMisterNegative::DemonLComponentBeginOverlap);

	demonColR->OnComponentBeginOverlap.AddDynamic(this,&AMisterNegative::DemonLComponentBeginOverlap);

	SwordCol->OnComponentBeginOverlap.AddDynamic(this,&AMisterNegative::SwordComponentBeginOverlap);

	SwordCol->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetDemonCollision(false);
}

// Called every frame
void AMisterNegative::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UE_LOG(LogTemp,Warning,TEXT("dissolveAnimValue : %f"), dissolveAnimValue);
	if (bisDissolve) // 사라지게
	{
		dissolveAnimValue += DeltaTime / 4;
		Demon->SetScalarParameterValueOnMaterials(TEXT("Animation"), dissolveAnimValue);

		if (dissolveAnimValue > 1)
		{
			bisDissolve = false;
			SetSwordMeshVisible(true);
		}
	}

	if (bisSetDissolve) // 나타나게
	{
		dissolveAnimValue -= DeltaTime ;
		Demon->SetScalarParameterValueOnMaterials(TEXT("Animation"), dissolveAnimValue);
		if (dissolveAnimValue < 0)
		{
			bisSetDissolve = false;
		}
	}

	FVector TargetLoc = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation();
	FVector WdigetLoc = SturnUi->GetComponentLocation();

	FVector dir = TargetLoc - WdigetLoc;
	dir.Normalize();

	FRotator rot = dir.ToOrientationRotator(); // 현재의 벡터가 향한 방향을 로테이션으로 반환
	SturnUi->SetWorldRotation(rot);
	
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

void AMisterNegative::SwordNiagaraVisible(bool chek)
{
	Naiagara->SetVisibility(chek);
}

void AMisterNegative::SetSwordMeshVisible(bool chek)
{
	Sword->SetVisibility(chek);
}

void AMisterNegative::NextFadeIn()
{
	if (Gmb)
	{
		Gmb->OnFadeIn();
	}	
}	

void AMisterNegative::CameraShake()
{
	GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(Cs_DemonAttack, 0.5f);
}

void AMisterNegative::DissolveAnim()
{
	bisDissolve = true;
	bisSetDissolve = false;
}

void AMisterNegative::SetDissolveAnim()
{
	bisSetDissolve = true;
	bisDissolve = false;
	SetSwordMeshVisible(false);
}

void AMisterNegative::SetDemonCollision(bool chek)
{
	if (chek)
	{
		demonColR->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		demonColL->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else
	{
		demonColR->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		demonColL->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AMisterNegative::NextLevel()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("SpiderPhase2"));// 스테이지 변경
}

void AMisterNegative::NextShake()
{
	GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(Cs_NextShake,1);
}

void AMisterNegative::MonsterSpawn()
{
	int random = FMath::RandRange(1, 3);

	if(random == 1)
	SpawnMonster->MonsterSpawn();
}

void AMisterNegative::Ending()
{
	Gmb->SetNegativeUI();
}

void AMisterNegative::SetUiVisble(bool chek)
{
	SturnUi->SetVisibility(chek);
}


void AMisterNegative::SwordComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASpiderMan* player = Cast<ASpiderMan>(OtherActor);

	if (player != nullptr)
	{
		SwordCol->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		player->LaunchCharacter(GetActorForwardVector() * 1000, false, false);
		UGameplayStatics::PlaySound2D(GetWorld(), HitSound);
		player->Damaged(1);
	}
}

void AMisterNegative::DemonRComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASpiderMan* player = Cast<ASpiderMan>(OtherActor);

	if (player != nullptr)
	{
		SetDemonCollision(false);
		player->LaunchCharacter(GetActorForwardVector() * 1000, false, false);
		UGameplayStatics::PlaySound2D(GetWorld(), HitSound);
		player->Damaged(1);
	}
}

void AMisterNegative::DemonLComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASpiderMan* player = Cast<ASpiderMan>(OtherActor);

	if (player != nullptr)
	{
		SetDemonCollision(false);
		
		player->LaunchCharacter(GetActorForwardVector() * 1000, false, false);
		UGameplayStatics::PlaySound2D(GetWorld(), HitSound);
		player->Damaged(1);
	}
}

