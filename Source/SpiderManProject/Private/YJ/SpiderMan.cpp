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
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicsEngine/PhysicsConstraintActor.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "YJ/Cable.h"
#include "YJ/PhyConstraintActor.h"


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

	/*ropeComp = CreateDefaultSubobject<UCableComponent>(TEXT("ropeComp"));

	ropeComp->SetupAttachment(RootComponent);*/


}

// Called when the game starts or when spawned
void ASpiderMan::BeginPlay()
{
	Super::BeginPlay();

	CableActor =GetWorld()->SpawnActor<ACable>(BP_CableActor);
	PConstraintActor = GetWorld()->SpawnActor<APhyConstraintActor>(BP_PhysicsConstraint);
	pc = GetWorld()->GetFirstPlayerController();
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
	
	if (pc)
	{
		// Get camera location and rotation
		FVector CameraLocation;
		FRotator CameraRotation;
		pc->GetPlayerViewPoint(CameraLocation, CameraRotation);

		// Calculate end location
		FVector EndLocation = CameraLocation + (CameraRotation.Vector() * MaxTraceDistance);

		// Perform line trace
		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this); // Ignore self in trace

		//bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, EndLocation, ECC_Visibility, Params);

		
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1)); //Wall
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel2)); //Catchable

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
			
			FTransform CharaSocketTranform = GetMesh()->GetSocketTransform(TEXT("hand_rSocket"), RTS_Actor);
			
			//CableActor->CableComp->EndLocation = GetActorLocation();
			CableActor->CableComp->SetWorldLocation(HitResult.ImpactPoint); //시작점을
			//CableActor->PhysicsConstraint->SetWorldLocation(HitResult.ImpactPoint);

			CableActor->StaticComp->SetWorldLocation(GetActorLocation());
			
			//끝점을 케이블의 static으로 하고 
			//CableActor->CableComp->SetAttachEndTo(this,TEXT("Mesh"),TEXT("hand_rSocket"));
			CableActor->CableComp->SetAttachEndToComponent(CableActor->StaticComp,NAME_None);
			
			//Physics Constraint 위치시키고 연결 시켜주기 
			PConstraintActor->SetActorLocation(HitResult.ImpactPoint);
			
			//PConstraintActor->PhysicsConstraintComponent->SetConstrainedComponents(CableActor->CableComp,NAME_None,this->GetCapsuleComponent(),NAME_None);

			PConstraintActor->PhysicsConstraintComponent->SetConstrainedComponents(CableActor->CableComp,NAME_None,CableActor->StaticComp,NAME_None);

			//CableActor->StaticComp->SetSimulatePhysics(true);

			//=> 문제점 물리로 연결지어주는 순간 (0,0,0) 으로 이동 => 왜 ?? 
			
			//auto end =CableActor->CableComp->GetAttachedComponent();

			//end->SetWorldLocation(GetActorLocation());
			
			//end->AttachToComponent(GetMesh(),FAttachmentTransformRules::KeepRelativeTransform,TEXT("hand_rSocket"));
			
			//FAttachmentTransformRules AttachmentTransformRules = FAttachmentTransformRules
			
			//CableActor->PhysicsConstraint->SetConstrainedComponents(CableActor->CableComp,NAME_None,CableActor->StaticComp,NAME_None);

			
			//this->AttachToComponent(end,FAttachmentTransformRules::SnapToTargetIncludingScale,TEXT("hand_rSocket"));
			
			//PConstraintActor->PhysicsConstraintComponent->ConstraintActor1 = CableActor;
			
			
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
	CableActor->CableComp->CableLength = length-500;

	// addforce를 하는 크기 
	/*FVector Dir = (GetActorLocation()-hookPoint);
	auto dot = UKismetMathLibrary::Dot_VectorVector(GetVelocity(), Dir);
	force = Dir.GetSafeNormal()*dot;
	GetCharacterMovement()->AddForce(force);
	GetCharacterMovement()->AirControl=1;*/

	FVector CameraLocation;
	FRotator CameraRotation;
	pc->GetPlayerViewPoint(CameraLocation, CameraRotation);
	
	// 카메라의 앞 방향 (Forward Vector)을 얻어옴
	FVector CameraForward = CameraRotation.Vector();

	// 캐릭터의 위치를 기준으로 힘을 가할 방향 설정
	FVector ForceDirection = CameraForward.GetSafeNormal();
	FVector Force = ForceDirection * 100;
	//GetCharacterMovement()->AddForce(Force);
	 
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
	//DetctedWall=true 하면 그 벽에 딱 달라붙기
}

void ASpiderMan::CatchActor()
{

}

TArray<AActor*> ASpiderMan::DetectEnemy()
{
	TArray<AActor*> DetectedActors;
	FVector MyLocation = GetActorLocation();
	FVector ForwardVector = GetActorForwardVector();

	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);

	for (AActor* Actor : AllActors)
	{
		if (Actor == this) continue;

		FVector DirectionToActor = Actor->GetActorLocation() - MyLocation;
		float DistanceToActor = DirectionToActor.Size();

		if (DistanceToActor <= DetectionRadius)
		{
			DirectionToActor.Normalize();
			float DotProduct = FVector::DotProduct(ForwardVector, DirectionToActor);
			float AngleToActor = FMath::Acos(DotProduct) * (180.0f / PI);

			if (AngleToActor <= DetectionAngle)
			{
				DetectedActors.Add(Actor);
			}
		}
	}

	return TArray<AActor*>();
}

void ASpiderMan::MyDrawDebugLine()
{
	FVector ForwardVector = GetActorForwardVector();
	FVector LeftBoundary = ForwardVector.RotateAngleAxis(-DetectionAngle, FVector::UpVector);
	FVector RightBoundary = ForwardVector.RotateAngleAxis(DetectionAngle, FVector::UpVector);

	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + LeftBoundary * DetectionRadius, FColor::Blue, false, 1.0f);
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + RightBoundary * DetectionRadius, FColor::Blue, false, 1.0f);
}




