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
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicsEngine/PhysicsConstraintActor.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "YJ/Cable.h"
#include "YJ/PhyConstraintActor.h"
#include "YJ/PointActor.h"
#include "YJ/SpiderFSMComponent.h"
#include "YJ/SpiderManAnimInstance.h"


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
	
	FSMComp = CreateDefaultSubobject<USpiderFSMComponent>(TEXT("FSMComp"));
}

// Called when the game starts or when spawned
void ASpiderMan::BeginPlay()
{
	Super::BeginPlay();

	CableActor =GetWorld()->SpawnActor<ACable>(BP_CableActor);
	PConstraintActor = GetWorld()->SpawnActor<APhyConstraintActor>(BP_PhysicsConstraint);
	pc = GetWorld()->GetFirstPlayerController();

	StartPointActor =GetWorld()->SpawnActor<APointActor>(BP_StartPoint);

	EndPointActor =GetWorld()->SpawnActor<APointActor>(BP_EndPoint);
	if (FSMComp)
	{
		FSMComp->SetState(EState::IDLE);
	}

	SpiderManAnim = Cast<USpiderManAnimInstance>(GetMesh()->GetAnimInstance());
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
	
	DetectCatchActor();
	
	if(CatchableObj&&bPressedCatchObj)
	{
		//있으면 lerp으로 다가오도록

		FVector CurrentLocation = FMath::Lerp(CatchableObj->GetActorLocation(), GetActorLocation(), DeltaTime * 5.f);
		CatchableObj->SetActorLocation(CurrentLocation);
		float dist = this->GetDistanceTo(CatchableObj);
		if(dist<=500.f)
		{
			//lerp 종료 -> 적이있으면 적에게 날라가도록 , 없으면 그냥 앞으로 던지기
			bPressedCatchObj=false;
		}
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
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ASpiderMan::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ASpiderMan::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASpiderMan::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASpiderMan::Look);

		EnhancedInputComponent->BindAction(HookAction, ETriggerEvent::Started, this, &ASpiderMan::FindHookPint);
		EnhancedInputComponent->BindAction(HookAction, ETriggerEvent::Completed, this, &ASpiderMan::CompletedHook);
		EnhancedInputComponent->BindAction(LMouseAction, ETriggerEvent::Started, this, &ASpiderMan::Attack);
		EnhancedInputComponent->BindAction(IA_CatchAction, ETriggerEvent::Started, this, &ASpiderMan::CatchActor);
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
		// 내 시야가 꼭 카메라의  ViewPoint는 아님
	// hit지점을 endlocation 으로 정하기
	if (pc )
	{
		// Get camera location and rotation
		FVector CameraLocation;
		FRotator CameraRotation;
		pc->GetPlayerViewPoint(CameraLocation, CameraRotation);

		// Calculate end location
		FVector EndLocation = CameraLocation + (CameraRotation.Vector() * MaxSwingTraceDistance);

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
			
			hooked =true;
			hookPoint = HitResult.ImpactPoint;
			
			FTransform CharaSocketTranform = GetMesh()->GetSocketTransform(TEXT("hand_rSocket"), RTS_Actor);
			

			CableActor->CableComp->SetWorldLocation(HitResult.ImpactPoint); //케이블의 시작점을 히트지점으로 설정
			
			StartPointActor->SetActorLocation(HitResult.ImpactPoint);

			//좀 더 높은곳에서 스윙하고싶다 ==>스윙하면서 위로올라가야함....속도도 붙어야함
		
			EndPointActor->SetActorLocation(GetActorLocation());
			EndPointActor->meshComp->SetWorldLocation(GetActorLocation());
			// meshComp 가 계층구조 자식이긴한데 위치가 부모 안따라가서 얘도 위치 정해주기 
			
			//끝점(EndPointActor)의 component를 케이블의 end으로 하고 
			CableActor->CableComp->SetAttachEndTo(EndPointActor,TEXT("meshComp"), NAME_None);

			//Physics Constraint도 위치시키고 연결 시켜주기 
			PConstraintActor->SetActorLocation(HitResult.ImpactPoint);
			
			
			PConstraintActor->PhysicsConstraintComponent->SetConstrainedComponents(StartPointActor->meshComp,NAME_None,EndPointActor->meshComp,NAME_None);

			//나자신 (캐릭터를) 끝점에 부착
			this->AttachToComponent(EndPointActor->meshComp,FAttachmentTransformRules::KeepWorldTransform,TEXT("hand_rSocket"));

			//this->AttachToActor(EndPointActor,FAttachmentTransformRules::KeepWorldTransform,TEXT("hand_rSocket"));
			//=> AttachToActor 이거 왜안됌..??


			newforce = GetVelocity()*5000.f + GetActorLocation();
			
			if (FSMComp)
			{
				FSMComp->SetState(EState::Swing);
			}
			FTimerHandle physicsTimer; 
			GetWorld()->GetTimerManager().SetTimer(physicsTimer, ([this]()->void
			{
				EndPointActor->meshComp->SetSimulatePhysics(true);
				LaunchCharacter(newforce,false,false);
				//왜 EndPointActor 에 addforce하면 문제 생기는것 ??
				GetCharacterMovement()->AirControl=1;
			}), 0.1f, false);
		}
		else
		{
			// Draw a debug line
			DrawDebugLine(GetWorld(), CameraLocation, EndLocation, FColor::Red, false, 1.0f, 0, 1.0f);
			
		}
	}
	
}

void ASpiderMan::CompletedHook()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	EndPointActor->meshComp->SetSimulatePhysics(false);
	EndPointActor->meshComp->SetRelativeLocation(FVector(0, 0, 0));
	if(FSMComp)
	{
		FSMComp->SetState(EState::IDLE);
	}
}

void ASpiderMan::CalculateSwing() //틱에서 작동
{
	//케이블의 길이 설정
	float length = (GetActorLocation() - hookPoint).Size();
	CableActor->CableComp->CableLength = length;
}

void ASpiderMan::DetectWall(FVector Direction)
{
	//벽을 감지하기 위해서 네방향으로 ray 발사해보기 
	FHitResult OutHit;
	FVector Start = GetActorLocation();
	FVector End = Start + Direction * DetectTraceLength;

	ECollisionChannel TraceChannel = ECC_GameTraceChannel1; //벽

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, TraceChannel, Params);
	//허공
	DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Red : FColor::Green, false, 1);
	
	if (bHit)
	{
		DrawDebugLine(GetWorld(), Start, OutHit.ImpactPoint, FColor::Red, false, 1);
		DetctedWall=true;
		hooked =false;
		if(FSMComp)
		{
			FSMComp->SetState(EState::IDLE);
		}
		//ImpactPoint 겉면의 지점
		//벽을 감지한다면 힘을 그만받도록 만들기
		    // 속도도 느리게
			// 클라이밍 되도록 만들기
	}
	else
	{
		DetctedWall = false;
	}

	
}

void ASpiderMan::ClimbingMode()
{
	//DetctedWall=true 하면 그 벽에 딱 달라붙기
}

void ASpiderMan::DetectCatchActor()
{
	// 내가 RAY를 써서 물건잡는게 아니라 범위안에 들어오는 물건을 잡는것
	 // 부채꼴의 시작 각도와 끝 각도를 계산

	// 내가 카메라 돌릴때마다 타겟팅 바뀜 ==>,,,,카메라에서 레이 발사 . 그리고 계속해서 작동 (공격하는 동안 )
		FVector CameraLocation;
		FRotator CameraRotation;
		pc->GetPlayerViewPoint(CameraLocation, CameraRotation);

		// Calculate end location
		FVector EndLocation = CameraLocation + (CameraRotation.Vector() * RayDetectObjDistance);

		// Perform line trace
		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this); // Ignore self in trace
	
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel2)); //Catchable

		bool bHitSphere = UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(),GetActorLocation(),EndLocation,DetectRadius,
		ObjectTypes,
		false,   // bTraceComplex
		TArray<AActor*>(), // Actors to ignore
		EDrawDebugTrace::None, // Draw debug
		HitResult,
		true);
		
		if (bHitSphere)
		{
			// Draw a debug line
			DrawDebugLine(GetWorld(), GetActorLocation(), EndLocation, FColor::Orange, false, 1.0f, 0, 1.0f);
			// Draw a debug point at the hit location
			DrawDebugPoint(GetWorld(), HitResult.Location, 10.0f, FColor::Purple, false, 1.0f);

			// Log the hit location
			UE_LOG(LogTemp, Log, TEXT("Hit location: %s"), *HitResult.Location.ToString());

			// hit 지점있으면 저장해두기
			CatchableObj = HitResult.GetActor();
			
		}
		else
		{
			// Draw a debug line
			//DrawDebugLine(GetWorld(), CameraLocation, EndLocation, FColor::Red, false, 1.0f, 0, 1.0f);
		}
	
}



void ASpiderMan::CatchActor() //Q버튼 누르면 실행할 함수
{
	bPressedCatchObj=!bPressedCatchObj;
	//DetectCatchActor 한 결과물을 가져오기
	if(CatchableObj)
	{
		//잡을수있는 물건이 있다면 ..
			//lerp 를 이용해서 나에게 다가오도록
			//물건과 나의거리가 가까워지면 lerp 종료 ==> 이건 tick 에서 작동
		
		
	}
	else
	{
		return;
	}
	
}



TArray<AActor*> ASpiderMan::DetectEnemy()
{
	TArray<AActor*> DetectedActors;
	FVector MyLocation = GetActorLocation();
	FVector ForwardVector = GetActorForwardVector();

	// SphereOverlapActors를 사용하여 반경 내의 액터들을 검색
	TArray<FOverlapResult> OverlapResults;
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(DetectionRadius);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bOverlap = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		MyLocation,
		FQuat::Identity,
		ECC_Pawn, // 필요한 콜리전 채널을 설정
		CollisionShape,
		QueryParams
	);

	if (bOverlap) //overlap 되는게 있다면 
	{
		for (const FOverlapResult& Result : OverlapResults)
		{
			AActor* Actor = Result.GetActor();
			if (Actor)
			{
				FVector DirectionToActor = Actor->GetActorLocation() - MyLocation;
				DirectionToActor.Normalize();

				float DotProduct = FVector::DotProduct(ForwardVector, DirectionToActor);
				float AngleToActor = FMath::Acos(DotProduct) * (180.0f / PI);

				if (AngleToActor <= DetectionAngle) //각도이내라면 
				{
					DetectedActors.Add(Actor);
					
					// 각도이내에 적이 많다 => 거리순으로 가장 가까운 적을 타겟팅하자
					
				}
			}
		}
	}

	return DetectedActors;
}

void ASpiderMan::MyDrawDebugLine()
{
	FVector ForwardVector = GetActorForwardVector();
	FVector LeftBoundary = ForwardVector.RotateAngleAxis(-DetectionAngle, FVector::UpVector);
	FVector RightBoundary = ForwardVector.RotateAngleAxis(DetectionAngle, FVector::UpVector);

	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + LeftBoundary * DetectionRadius, FColor::Blue, false, 1.0f);
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + RightBoundary * DetectionRadius, FColor::Blue, false, 1.0f);
}

void ASpiderMan::Jump()
{
	Super::Jump();
	if(GetCharacterMovement()->IsFalling())
	{
		DoubleJump();
	}
}

void ASpiderMan::StopJumping()
{
	Super::StopJumping();

	
}

void ASpiderMan::DoubleJump()
{

	//점프버튼을 두번눌렀을때 할일
		// 살짝위로 점프
		// 
		// 나의 앞방향으로 ray 발사 그지점으로 내가 앞으로 쭉 lerp 이동 
	
	FVector CameraLocation;
	FRotator CameraRotation;
	pc->GetPlayerViewPoint(CameraLocation, CameraRotation);

	// Calculate end location
	FVector EndLocation = CameraLocation + (CameraRotation.Vector() * MaxSwingTraceDistance);

	// Perform line trace
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); // Ignore self in trace

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, EndLocation, ECC_Visibility, Params);


	if (bHit)
	{
		// Draw a debug line
		DrawDebugLine(GetWorld(), GetActorLocation(), EndLocation, FColor::Silver, false, 1.0f, 0, 1.0f);
		// Draw a debug point at the hit location
		DrawDebugPoint(GetWorld(), HitResult.Location, 10.0f, FColor::Red, false, 1.0f);

		// Log the hit location
		UE_LOG(LogTemp, Log, TEXT("Hit location: %s"), *HitResult.Location.ToString());

		//벽이라면
		if(HitResult.GetActor()->ActorHasTag(TEXT("Wall")))
		{
			
			
   			DoubleTargetVector=HitResult.ImpactPoint;

			CableActor->CableComp->SetWorldLocation(HitResult.ImpactPoint); //케이블의 시작점을 히트지점으로 설정

			StartPointActor->SetActorLocation(HitResult.ImpactPoint);
			
		
			//끝점(EndPointActor)의 component를 메쉬의 끝점
			CableActor->CableComp->SetAttachEndTo(this,TEXT("Mesh"),TEXT("hand_rSocket"));

			//lerp이동 처리를 여기서 하기
			if(FSMComp)
			{
				FSMComp->SetState(EState::DoubleJump);
			}

			UE_LOG(LogTemp, Log, TEXT("Hit ImpactPoint: %s"), *HitResult.ImpactPoint.ToString());
			 
			
		}
		
	}
	else
	{
		// Draw a debug line
		DrawDebugLine(GetWorld(), CameraLocation, EndLocation, FColor::Red, false, 1.0f, 0, 1.0f);
	}
}

void ASpiderMan::Attack()
{
	if(FSMComp)
	{
		FSMComp->SetState(EState::ATTACK);
	}
	
	if(SpiderManAnim)
	{
		SpiderManAnim->SetAnimState(EAnimState::ATTACKAnim);
	}
	//fsm 에서  attack 중 tick으로 작동해야할것 처리하기
}

void ASpiderMan::Damaged(float value)
{
	CurHP-=value;
	UE_LOG(LogTemp,Warning, TEXT("Spider Damaged, curHP : %f"), CurHP);
}





