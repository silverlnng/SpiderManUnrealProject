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
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "YJ/Cable.h"
#include "YJ/PhyConstraintActor.h"
#include "YJ/PointActor.h"
#include "YJ/SpiderFSMComponent.h"
#include "YJ/SpiderManAnimInstance.h"
#include "Camera/PlayerCameraManager.h"
#include "PSH/MisterNegative.h"
#include "PSH/MisterNegativeFSM.h"


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

	CableComp =CreateDefaultSubobject<UCableComponent>(TEXT("CableComponent"));

	CableComp->SetupAttachment(GetMesh(),TEXT("hand_rSocket"));
	
	
	FSMComp = CreateDefaultSubobject<USpiderFSMComponent>(TEXT("FSMComp"));

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));

	IsAttacking = false;
	MaxCombo = 4;
	AttackStartComboState();
	AttackEndComboState();
	
}

void ASpiderMan::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	SpiderManAnim = Cast<USpiderManAnimInstance>(GetMesh()->GetAnimInstance());

	SpiderManAnim->OnMontageEnded.AddDynamic(this, &ASpiderMan::OnAttackMontageEnded);

	SpiderManAnim->OnNextAttackCheck.AddLambda([this]() -> void {
		
		CanNextCombo = false;

		if (IsComboInputOn) {
			AttackStartComboState();
			SpiderManAnim->JumpToAttackMontageSection(CurrentCombo);
		}
	});
	
	SpiderManAnim->OnAttackHitCheck.AddUObject(this, &ASpiderMan::ComboAttackCheck);
	SpiderManAnim->OnAirAttackTriggerCheck.AddUObject(this, &ASpiderMan::AirAttackTriggerCheck);

	bIsDodging = false;
	DodgeDistance = 600.f;  // Dodge 이동 거리
	DodgeCooldown = 0.5f;   // Dodge 쿨다운 시간
	LastDodgeTime = -DodgeCooldown; // 초기화
	
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
	
	

	//피직스 핸들을 만들고 항상 나의 위치에 오도록 만든다
	PhysicsHandle->SetTargetLocation(GetActorLocation());

	CableComp->SetVisibility(false);

	CameraManager =GetWorld()->GetFirstPlayerController()->PlayerCameraManager;

	//FTimerHandle FindHookPoint_Auto;
	//GetWorldTimerManager().SetTimer(FindHookPoint_Auto,this,&ASpiderMan::FindHookPoint_Auto,0.5f,true,-1);

	BossEnemy =Cast<AMisterNegative>(UGameplayStatics::GetActorOfClass(GetWorld(),AMisterNegative::StaticClass()));
	
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
		CalculateSwing(hookPoint);
	}
	
	DetectCatchActor();
	
	if(CatchableObj && bPressedCatchObj)
	{
		//있으면 lerp으로 다가오도록

		FVector CurrentLocation = FMath::Lerp(CatchableObj->GetActorLocation(), GetActorLocation(), DeltaTime * 5.f);
		CatchableObj->SetActorLocation(CurrentLocation);
		float dist = this->GetDistanceTo(CatchableObj);

		CalculateSwing(CatchableObj->GetActorLocation());
		
		if(dist<=200.f)
		{
			

			// CatchableObj 을 나에게 부착 그리고 회전 !
			CatchableObj->K2_AttachToActor(this,NAME_None,EAttachmentRule::KeepWorld,EAttachmentRule::KeepRelative,EAttachmentRule::KeepRelative,true);
			
			// 한번 나를 회전시키고
			bRotateSpiderMan =true;
			// 적이있으면 적에게 날라가도록 , 없으면 그냥 앞으로 던지기

			//lerp 종료 -
			bPressedCatchObj=false;
		}
	}

	if(bRotateSpiderMan)
	{
		RotateSpiderMan(DeltaTime);
	}
	
	camForce = UKismetMathLibrary::GetForwardVector(CameraManager->GetCameraRotation());
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

		EnhancedInputComponent->BindAction(HookAction, ETriggerEvent::Started, this, &ASpiderMan::FindHookPoint_pushShift);
		EnhancedInputComponent->BindAction(HookAction, ETriggerEvent::Completed, this, &ASpiderMan::CompletedHook);
		EnhancedInputComponent->BindAction(LMouseAction, ETriggerEvent::Started, this, &ASpiderMan::ComboAttack);
		EnhancedInputComponent->BindAction(IA_CatchAction, ETriggerEvent::Started, this, &ASpiderMan::CatchActor);
		EnhancedInputComponent->BindAction(IA_LCtrl, ETriggerEvent::Started, this, &ASpiderMan::Dodge);
	}
	else
	{
		//UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASpiderMan::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	MovementVector = Value.Get<FVector2D>();
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

void ASpiderMan::Dodge(const FInputActionValue& Value)
{
	if (bIsDodging || GetWorld()->GetTimeSeconds() - LastDodgeTime < DodgeCooldown)
	{
		return;
	}

	bIsDodging = true;
	LastDodgeTime = GetWorld()->GetTimeSeconds();

	/*FVector DodgeDirection = FVector::ZeroVector;
	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		float ForwardValue = GetInputAxisValue("MoveForward");
		float RightValue = GetInputAxisValue("MoveRight");

		if (FMath::Abs(MovementVector.X) > 0.1f || FMath::Abs(MovementVector.Y) > 0.1f)
		{
			DodgeDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) * MovementVector.X +
						   FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y) * MovementVector.Y;
			DodgeDirection.Normalize();
		}
		else
		{
			DodgeDirection = GetActorForwardVector();
		}
	}

	FVector DodgeVelocity = DodgeDirection * DodgeDistance;
	LaunchCharacter(DodgeVelocity, true, true);*/

	// 일정 시간 후에 dodge 상태를 false로 설정

	//PlayAnimMontage(SpiderManAnim->dodgeMontage);
	
	FTimerHandle UnusedHandle;
	GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &ASpiderMan::StopDodge, 0.1f, false);
}

void ASpiderMan::StopDodge()
{
	bIsDodging = false;
}

#pragma endregion BasicMove 

#pragma region Hook

void ASpiderMan::FindHookPoint_pushShift()
{
	// 버튼을 누르면 내 시야로 ray를 발사하여 hit지점을 구하고 ,
		// 내 시야가 꼭 카메라의  ViewPoint는 아님
	// hit지점을 endlocation 으로 정하기

	// State 에 따라 다르게 작동하도록 하기
	if(FSMComp->LevelState == ELevelState::BOSSENEMY)
	{

		//보스를 찾아서 보스 위치 z 값만 올려서 hook point 잡기


		hooked = true;

		FVector BossPoint = BossEnemy->GetActorLocation();
		float OffsetBoss = BossPoint.Z + 3000.f;
		BossPoint = FVector(BossPoint.X,BossPoint.Y,OffsetBoss);
		hookPoint = BossPoint;

		DrawDebugLine(GetWorld(), GetActorLocation(), hookPoint, FColor::Yellow, false, 1.0f, 0, 1.0f);

		CableActor->CableComp->SetVisibility(true);

		CableActor->CableComp->SetWorldLocation(hookPoint); //케이블의 시작점을 히트지점으로 설정

		StartPointActor->SetActorLocation(hookPoint);

		//좀 더 높은곳에서 스윙하고싶다 ==>스윙하면서 위로올라가야함....속도도 붙어야함
		FVector offset = GetActorLocation() + GetActorUpVector() * 100.f;

		EndPointActor->SetActorLocation(offset);
		EndPointActor->meshComp->SetWorldLocation(offset);
		// meshComp 가 계층구조 자식이긴한데 위치가 부모 안따라가서 얘도 위치 정해주기 

		//끝점(EndPointActor)의 component를 케이블의 end으로 하고 
		//CableActor->CableComp->SetAttachEndTo(EndPointActor,TEXT("meshComp"), NAME_None);

		CableActor->CableComp->SetAttachEndTo(this,TEXT("Mesh"), TEXT("hand_rSocket"));
		
		//Physics Constraint도 위치시키고 연결 시켜주기 
		PConstraintActor->SetActorLocation(hookPoint);


		PConstraintActor->PhysicsConstraintComponent->SetConstrainedComponents(
			StartPointActor->meshComp, NAME_None, EndPointActor->meshComp, NAME_None);

		//나자신 (캐릭터를) 끝점에 부착
		this->AttachToComponent(EndPointActor->meshComp, FAttachmentTransformRules::KeepWorldTransform,
		                        TEXT("hand_rSocket"));
		this->SetActorRelativeLocation(FVector(0, 0, 0));

		// 매달리는 애니메이션 실행 


		//newforce = GetVelocity() * 100.f + GetActorLocation();
		newforce = hookPoint-GetActorLocation();
		 
		this->GetCapsuleComponent()->SetCapsuleHalfHeight(20);

		FSMComp->SetState(EState::SWING);

		FTimerHandle physicsTimer;
		GetWorld()->GetTimerManager().SetTimer(physicsTimer, ([this]()-> void
		{
			EndPointActor->meshComp->SetSimulatePhysics(true);
			
			EndPointActor->meshComp->AddForce(newforce*1000.f);			
	
			GetCharacterMovement()->AirControl = 1.f;
			
			FVector camForce = UKismetMathLibrary::GetForwardVector(CameraManager->GetCameraRotation());
			
			EndPointActor->meshComp->AddForce(camForce * 100, NAME_None, true);
			
		}), 0.01f, false);

		GetWorld()->GetTimerManager().SetTimer(addforceTimer, ([this]()-> void
		{
			EndPointActor->meshComp->AddForce(camForce * 10000.f, NAME_None, true);
			//EndPointActor->meshComp->AddForce(newforce*100.f, NAME_None, true);
		}), 0.5f, true, 0.3f);
		
		
	}
	else //  스테이트 에 따라 다르게 hook point 잡기 
	{
		if (pc)
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

			bool bHitSphere = UKismetSystemLibrary::SphereTraceSingleForObjects(
				GetWorld(), GetActorLocation(), EndLocation, Radius,
				ObjectTypes,
				false, // bTraceComplex
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

				hooked = true;
				hookPoint = HitResult.ImpactPoint;

				FTransform CharaSocketTranform = GetMesh()->GetSocketTransform(TEXT("hand_rSocket"), RTS_Actor);

				CableActor->CableComp->SetVisibility(true);

				CableActor->CableComp->SetWorldLocation(HitResult.ImpactPoint); //케이블의 시작점을 히트지점으로 설정

				StartPointActor->SetActorLocation(HitResult.ImpactPoint);

				//좀 더 높은곳에서 스윙하고싶다 ==>스윙하면서 위로올라가야함....속도도 붙어야함
				FVector offset = GetActorLocation() + GetActorUpVector() * 50.f;

				EndPointActor->SetActorLocation(offset);
				EndPointActor->meshComp->SetWorldLocation(offset);
				// meshComp 가 계층구조 자식이긴한데 위치가 부모 안따라가서 얘도 위치 정해주기 

				FVector handSocket_Loc  = GetMesh()->GetSocketLocation(TEXT("hand_rSocket"));

				EndPointActor->SetActorLocation(handSocket_Loc);
				EndPointActor->meshComp->SetWorldLocation(handSocket_Loc);
				
				//끝점(EndPointActor)의 component를 케이블의 end으로 하고 
				//CableActor->CableComp->SetAttachEndTo(EndPointActor,TEXT("meshComp"), NAME_None);

				CableActor->CableComp->SetAttachEndTo(this,TEXT("Mesh"), TEXT("hand_rSocket"));

				//Physics Constraint도 위치시키고 연결 시켜주기 
				PConstraintActor->SetActorLocation(HitResult.ImpactPoint);


				PConstraintActor->PhysicsConstraintComponent->SetConstrainedComponents(
					StartPointActor->meshComp, NAME_None, EndPointActor->meshComp, NAME_None);

				//나자신 (캐릭터를) 끝점에 부착
					// 나의 handSocket 을 endpoint에 부착 
				this->AttachToComponent(EndPointActor->meshComp, FAttachmentTransformRules::KeepWorldTransform,
				                        TEXT("hand_rSocket"));

				
				
				
				
				this->SetActorRelativeLocation(FVector(0, 0, 0));

				// 매달리는 애니메이션 실행 


				newforce = GetVelocity() * 100.f + GetActorLocation();

				this->GetCapsuleComponent()->SetCapsuleHalfHeight(20);

				FSMComp->SetState(EState::SWING);

				FTimerHandle physicsTimer;
				GetWorld()->GetTimerManager().SetTimer(physicsTimer, ([this]()-> void
				{
					EndPointActor->meshComp->SetSimulatePhysics(true);
					LaunchCharacter(newforce, false, false);
					//왜 EndPointActor 에 addforce하면 문제 생기는것 ??
					GetCharacterMovement()->AirControl = 1.f;
					FVector camForce = UKismetMathLibrary::GetForwardVector(CameraManager->GetCameraRotation());
					EndPointActor->meshComp->AddForce(camForce * 100, NAME_None, true);
				}), 0.01f, false);

				GetWorld()->GetTimerManager().SetTimer(addforceTimer, ([this]()-> void
				{
					EndPointActor->meshComp->AddForce(camForce * 10000.f, NAME_None, true);
				}), 1.f, true, 1.f);
			}
			else
			{
				// Draw a debug line
				DrawDebugLine(GetWorld(), CameraLocation, EndLocation, FColor::Red, false, 1.0f, 0, 1.0f);
			}
		}
	}
	
	
}

void ASpiderMan::CompletedHook() 
{
	hooked = false;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	EndPointActor->meshComp->SetSimulatePhysics(false);
	EndPointActor->meshComp->SetRelativeLocation(FVector(0, 0, 0));
	this->GetCapsuleComponent()->SetCapsuleHalfHeight(90);
	FSMComp->SetState(EState::IDLE);
	FSMComp->IdleState();
	GetWorldTimerManager().ClearTimer(addforceTimer);
	
}

#pragma endregion Hook

void ASpiderMan::CalculateSwing(FVector loc) //틱에서 작동
{
	//케이블의 길이 설정
	float length = (GetActorLocation() - loc).Size();
	CableActor->CableComp->CableLength = length-300;
}

void ASpiderMan::DetectWall(FVector Direction)
{
	//벽을 감지하기 위해서 네방향으로 ray 발사해보기 
	FHitResult OutHit;
	FVector Start = GetActorLocation();
	FVector End = Start + Direction * DetectTraceLength;

	ECollisionChannel TraceChannel = ECC_Visibility; 
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, TraceChannel, Params);
	//허공
	DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Red : FColor::Green, false, 1);
	
	if (bHit)
	{
		//OutHit 결과물이 Wall , boss
		if(OutHit.GetActor()->ActorHasTag(TEXT("Wall")) || OutHit.GetActor()->ActorHasTag(TEXT("Boss")) )
		{
			DrawDebugLine(GetWorld(), Start, OutHit.ImpactPoint, FColor::Red, false, 1);
			DetctedWall=true;
			hooked =false;
			CableActor->CableComp->SetVisibility(false);
			if(FSMComp)
			{
				FSMComp->SetState(EState::IDLE);
			}
			//ImpactPoint 겉면의 지점
			//벽을 감지한다면 힘을 그만받도록 만들기
			// 속도도 느리게
			// 클라이밍 되도록 만들기
		}
		
		
		
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

#pragma region ThrowCatchableActor

void ASpiderMan::FindHookPoint_Auto()
{
	//타이머로 자동으로 가까운 hook 타겟 찾기
	
	TArray<AActor*> DetectedActors;
	FVector MyLocation = GetActorLocation();
	FVector ForwardVector = GetActorForwardVector();

	// SphereOverlapActors를 사용하여 반경 내의 액터들을 검색
	TArray<FOverlapResult> OverlapHookPointResults;
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(DetectHookPointRadius);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bOverlaped = GetWorld()->OverlapMultiByChannel(
		OverlapHookPointResults,
		MyLocation,
		FQuat::Identity,
		ECC_GameTraceChannel6, // 필요한 콜리전 채널을 설정
		CollisionShape,
		QueryParams
	);

	if (bOverlaped) //overlap 되는게 있다면 
	{
		for (const FOverlapResult& Result : OverlapHookPointResults)
		{
			AActor* Actor = Result.GetActor();
			float dist =FVector::Dist(Actor->GetActorLocation(),GetActorLocation());
			
			DrawDebugSphere(GetWorld(),Actor->GetActorLocation(),100, 13, FColor::Orange, true, -1, 0, 2);
			
			//거리가 가장 가까운거로 change.
			//Hookpoint_Nearest =
			if(dist<prevDistance) // 기존 것보다 더 작으면 
			{
				//prevDistance = dist; //이게 계속 작아지면 안될려나...
				Hookpoint_Nearest = Actor;
				DrawDebugSphere(GetWorld(),Hookpoint_Nearest->GetActorLocation(),100, 13, FColor::Purple, true, -1, 0, 2);
			}
			
		}
	}	
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
		// hit 지점있으면 저장해두기
		
		//끝점(EndPointActor)의 component를 케이블의 end으로 하고 
	
		// CatchableObj 를 나의 자식으로 두어서 내가 회전하면 같이 따라 회전하도록 만들고 싶다
		CableComp->SetVisibility(true);
		CableComp->SetAttachEndTo(CatchableObj,"BOX",NAME_None);
		
	}
	else
	{
		return;
	}
	
}

void ASpiderMan::RotateSpiderMan(float time)
{
	curtime+=time;
	AddActorLocalRotation(FRotator(0,time*360,0));
	if(curtime>=1.f)
	{
		// 틱 종료 
		bRotateSpiderMan=false;

		ThrowCatchActor();

		curtime=0;
		//물건 던지기 
	}
}

void ASpiderMan::ThrowCatchActor()
{
	CatchableObj->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	CableComp->SetVisibility(false);

	UStaticMeshComponent* temp =Cast<UStaticMeshComponent>(CatchableObj->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	
	temp->SetSimulatePhysics(true);

	//GetWorldTimerManager().SetTimer(addforceCatchItemTimer,([temp,this]()->void
	//{
	////temp->AddForce(this->GetActorForwardVector()*10000.f);
	//
	//}), 1.f, false, 1.f);
	temp->AddImpulse(this->GetActorForwardVector() * 10000.f);
	CatchableObj=nullptr;
	
}



#pragma endregion ThrowCatchableActor

TArray<AActor*> ASpiderMan::DetectEnemy()
{
	TArray<AActor*> DetectedActors;
	FVector MyLocation = GetActorLocation();
	FVector ForwardVector = GetActorForwardVector();

	// SphereOverlapActors를 사용하여 반경 내의 액터들을 검색
	TArray<FOverlapResult> OverlapResults;
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(DetectEnemyRadius);

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

	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + LeftBoundary * DetectEnemyRadius, FColor::Blue, false, 1.0f);
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + RightBoundary * DetectEnemyRadius, FColor::Blue, false, 1.0f);
}

#pragma region Jump,DoubleJump

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

	if(FSMComp->LevelState == ELevelState::BOSSENEMY) //보스에게 
	{
		CableActor->CableComp->SetVisibility(true);

		DoubleTargetVector=BossEnemy->GetActorLocation();

		CableActor->CableComp->SetWorldLocation(DoubleTargetVector); //케이블의 시작점을 히트지점으로 설정

		StartPointActor->SetActorLocation(DoubleTargetVector);
			
		
		//끝점(EndPointActor)의 component를 메쉬의 끝점
		CableActor->CableComp->SetAttachEndTo(this,TEXT("Mesh"),TEXT("hand_rSocket"));

		//lerp이동 처리를 여기서 하기
		if(FSMComp)
		{
			FSMComp->SetState(EState::DoubleJump);
		}

	
	}
	else // 보스에네미 상태가 아닐때 
	{
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
			
				CableActor->CableComp->SetVisibility(true);

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
	
	
}

#pragma endregion Jump,DoubleJump

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
	
	//Damage 애니 실행
	
	// 콤보공격 멈추고 초기화
	
	AttackEndComboState();
	UE_LOG(LogTemp,Warning, TEXT("Spider Damaged, curHP : %f"), CurHP);
}

#pragma region ComboAttack


void ASpiderMan::ComboAttack()
{
	// 스윙공격 중 들어온 입력이라면
		// 스윙공격을 
	
	if (IsAttacking)	// 공격이 진행중에 들어온 입력이라면  
	{
		if (CanNextCombo)
		{
			IsComboInputOn = true;
		}
	}
	else { 
		AttackStartComboState();
		SpiderManAnim->PlayAttackMontage();
		SpiderManAnim->JumpToAttackMontageSection(CurrentCombo);
		IsAttacking = true;
	}
}

void ASpiderMan::ComboAttackCheck()
{
	// 애니메이션 실행중 
	// sweep 으로 내가 enemy를 damage를 주는지 체크
	FHitResult HitResult_Hand_R;
	
	FCollisionQueryParams Params;
	//Params.AddIgnoredActor(Me);
	
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult_Hand_R,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * AttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_Visibility,
		FCollisionShape::MakeSphere(AttackRadius),
		Params);

#if ENABLE_DRAW_DEBUG
	FVector TraceVec = GetActorForwardVector() * AttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = AttackRange * 0.5f + AttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 1.0f;

	DrawDebugCapsule(GetWorld(),
		Center,
		HalfHeight,
		AttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime);

#endif
	if (bResult) {
		if (::IsValid(HitResult_Hand_R.GetActor()))
		{
			UE_LOG(LogTemp,Warning, TEXT("Hit Actor Name : %s"), *HitResult_Hand_R.GetActor()->GetName());
			AMisterNegative* MisterNegative = Cast<AMisterNegative>(HitResult_Hand_R.GetActor());
			if(MisterNegative)
			{
				
				auto NegativeFSM = MisterNegative->GetComponentByClass<UMisterNegativeFSM>();
				NegativeFSM->Dameged(1);
			}
		}
	}
	
}

void ASpiderMan::AirAttackTriggerCheck()
{
	// 애니메이션 실행중 
	// sweep 으로 내가 enemy를 damage를 주는지 체크
	FHitResult HitResult_Hand_R;
	
	FCollisionQueryParams Params;
	//Params.AddIgnoredActor(Me);
	
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult_Hand_R,
		GetActorLocation(),
		GetActorLocation() + GetActorUpVector() * AttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_Visibility,
		FCollisionShape::MakeSphere(AttackRadius),
		Params);

#if ENABLE_DRAW_DEBUG
	FVector TraceVec = GetActorUpVector() * AttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = AttackRange * 0.5f + AttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Purple : FColor::Black;
	float DebugLifeTime = 1.0f;

	DrawDebugCapsule(GetWorld(),
		Center,
		HalfHeight,
		AttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime);

#endif
	if (bResult) {
		if (::IsValid(HitResult_Hand_R.GetActor()))
		{
			UE_LOG(LogTemp,Warning, TEXT("Hit Actor Name : %s"), *HitResult_Hand_R.GetActor()->GetName());
			AMisterNegative* MisterNegative = Cast<AMisterNegative>(HitResult_Hand_R.GetActor());
			if(MisterNegative)
			{
				
				auto NegativeFSM = MisterNegative->GetComponentByClass<UMisterNegativeFSM>();
				NegativeFSM->Dameged(1);

				// 적을 공중에 띄우고 -> 중력 0으로 해야 안떨어질듯 ??
				MisterNegative->LaunchCharacter(GetActorUpVector()*1000.f,false,false);
				// 에어콤보를 시작하기 
				
			}
		}
	}
}

void ASpiderMan::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	IsAttacking = false;
	AttackEndComboState();
}

void ASpiderMan::AttackStartComboState() //공격 시작할때 속성 설정
{
	IsComboInputOn = false;
	CanNextCombo = true;
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
	FSMComp->SetState(EState::ATTACK);
}

void ASpiderMan::AttackEndComboState() // 공격끝날떄 속성설정
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
	FSMComp->SetState(EState::IDLE);
}

#pragma endregion  ComboAttack




