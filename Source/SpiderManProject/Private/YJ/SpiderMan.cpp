// Fill out your copyright notice in the Description page of Project Settings.


#include "YJ/SpiderMan.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "CableComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "YJ/Cable.h"

// Sets default values
ASpiderMan::ASpiderMan()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ropeComp = CreateDefaultSubobject<UCableComponent>(TEXT("ropeComp"));

	ropeComp->SetupAttachment(RootComponent);


}

// Called when the game starts or when spawned
void ASpiderMan::BeginPlay()
{
	Super::BeginPlay();

	CableActor =GetWorld()->SpawnActor<ACable>(BP_CableActor);
}

// Called every frame
void ASpiderMan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector Forward = GetActorForwardVector();
	FVector Right = GetActorRightVector();
	DetectWall(Forward);
	//DetectWall(Right);
	if(hooked&& !DetctedWall)
	{
		CalculateSwing();
	}
}

#pragma region BasicMove

// Called to bind functionality to input
void ASpiderMan::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASpiderMan::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASpiderMan::Look);

		EnhancedInputComponent->BindAction(LMouseAction, ETriggerEvent::Triggered, this, &ASpiderMan::FindHookPint);
	}
	else
	{
		//UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASpiderMan::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ASpiderMan::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

#pragma endregion BasicMove 

void ASpiderMan::FindHookPint()
{
	// 버튼을 누르면 내 시야로 ray를 발사하여 hit지점을 구하고 ,
	// hit지점을 endlocation 으로 정하기
	
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		// Get camera location and rotation
		FVector CameraLocation;
		FRotator CameraRotation;
		PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

		// Calculate end location
		FVector EndLocation = CameraLocation + (CameraRotation.Vector() * MaxTraceDistance);

		// Perform line trace
		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this); // Ignore self in trace

		//bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, EndLocation, ECC_Visibility, Params);

		
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1));
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel2));

		bool bHitSphere = UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(),GetActorLocation(),EndLocation,Radius,
		ObjectTypes,
		false,   // bTraceComplex
		TArray<AActor*>(), // Actors to ignore
		EDrawDebugTrace::ForDuration, // Draw debug
		HitResult,
		true);
		
		if (bHitSphere)
		{
			// Draw a debug line
			DrawDebugLine(GetWorld(), GetActorLocation(), EndLocation, FColor::Green, false, 1.0f, 0, 1.0f);
			// Draw a debug point at the hit location
			DrawDebugPoint(GetWorld(), HitResult.Location, 10.0f, FColor::Red, false, 1.0f);

			// Log the hit location
			UE_LOG(LogTemp, Log, TEXT("Hit location: %s"), *HitResult.Location.ToString());

			//

			hooked =true;
			hookPoint = HitResult.ImpactPoint;
			
			FTransform temp = GetMesh()->GetSocketTransform(TEXT("hand_rSocket"), RTS_Actor);
			
			CableActor->CableComp->SetWorldLocation(HitResult.ImpactPoint);
			CableActor->CableComp->SetAttachEndTo(this,TEXT("Mesh"),TEXT("hand_rSocket"));
			
			
		}
		else
		{
			// Draw a debug line
			DrawDebugLine(GetWorld(), CameraLocation, EndLocation, FColor::Red, false, 1.0f, 0, 1.0f);
		}
	}
	
}

void ASpiderMan::CalculateSwing() //틱에서 작동
{
	//케이블의 길이 설정
	float length = (GetActorLocation() - hookPoint).Size();
	CableActor->CableComp->CableLength = length;

	// addforce를 하는 크기 
	FVector temp = GetActorLocation()-hookPoint;
	FVector veloc  =GetVelocity()*0.5;
	auto dot = UKismetMathLibrary::Dot_VectorVector(veloc, temp);
	force = temp.GetSafeNormal()*dot;
	GetCharacterMovement()->AddForce(force);
	GetCharacterMovement()->AirControl=1;
	 
}

void ASpiderMan::DetectWall(FVector Direction)
{
	//벽을 감지하기 위해서 네방향으로 ray 발사해보기 
	FHitResult OutHit;
	FVector Start = GetActorLocation();
	FVector End = Start + Direction * DetectTraceLength;

	ECollisionChannel TraceChannel = ECC_GameTraceChannel1;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, TraceChannel, Params);
	if (bHit)
	{
		DrawDebugLine(GetWorld(), Start, OutHit.ImpactPoint, FColor::Red, false, 1);
		DetctedWall=true;
		hooked =false;
		//ImpactPoint 겉면의 지점
		//벽을 감지한다면 힘을 그만받도록 만들기
		    // 속도도 느리게
			// 클라이밍 되도록 만들기
	}
	else
	{
		DetctedWall = false;
	}

	//허공
	DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Red : FColor::Green, false, 1);
	
}

void ASpiderMan::ClimbingMode()
{
	
}

//DetctedWall=true 하면 그 벽에 딱 달라붙기
