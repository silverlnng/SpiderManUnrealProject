// Fill out your copyright notice in the Description page of Project Settings.


#include "YJ/SpiderFSMComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "PSH/MisterNegative.h"
#include "PSH/MisterNegativeFSM.h"
#include "YJ/SpiderMan.h"
#include "YJ/Cable.h"
#include "CableComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "YJ/SpiderManAnimInstance.h"

// Sets default values for this component's properties
USpiderFSMComponent::USpiderFSMComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USpiderFSMComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Me = Cast<ASpiderMan>(GetOwner());

	BossEnemy = Cast<AMisterNegative>(UGameplayStatics::GetActorOfClass(GetWorld(),AMisterNegative::StaticClass()));
	
}


// Called every frame
void USpiderFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	/*FString mystate = UEnum::GetValueAsString(State);

	//위치에 문자 띄우기
	DrawDebugString(GetWorld(),GetOwner()->GetActorLocation(),mystate, nullptr,FColor::Yellow,0,true);*/
	// ...

	//거리좀 출력하고싶다
	
	
	switch ( State )
	{
	case EState::IDLE:		TickIdle(DeltaTime);		break;
	case EState::SWING:		TickSwing(DeltaTime);		break;
	case EState::DoubleJump:TickDoubleJump(DeltaTime);	break;
	case EState::ATTACK:	TickAttack(DeltaTime);		break;
	case EState::DAMAGE:	TickDamage(DeltaTime);		break;
	case EState::DIE:		TickDie(DeltaTime);			break;
	}
}

void USpiderFSMComponent::TickIdle(const float& DeltaTime)
{
	
}

void USpiderFSMComponent::TickDoubleJump(const float& DeltaTime)
{
	//회전
	FVector start = Me->GetActorLocation();
	FVector end = BossEnemy->GetActorLocation();
	
	FRotator rot = UKismetMathLibrary::FindLookAtRotation(start,end);

	FRotator interpRot = UKismetMathLibrary::RInterpTo_Constant(Me->GetActorRotation(),rot,DeltaTime,100.f);
	FRotator newRot = FRotator(0,interpRot.Yaw,0);
	Me->SetActorRotation(newRot);

	// 이동하기 전에 줄을 당기는 애니메이션 실행
	

	// 애니메이션 실행을 다끝내는 신호 받고 
		// Me 를 타겟점으로 lerp하게 이동 
	FVector CurrentLocation = FMath::Lerp(Me->GetActorLocation(), Me->DoubleTargetVector, DeltaTime*5.f);
	Me->SetActorLocation(CurrentLocation);
	float dist = FVector::Dist(Me->GetActorLocation(),Me->DoubleTargetVector);

	float length = (Me->GetActorLocation() - Me->DoubleTargetVector).Size();
	Me->CableActor->CableComp->CableLength = length - 500;

	//다 도착하면 idle으로 다시 => 그냥 시간이 지나면 idle상태로 돌아가도록


	if (Me->SpiderManAnim->DoubleJumpTargetIsBoss)
	{
		if (dist <= 380.f)
		{
			//타겟이 보스일때만

			Me->GetCharacterMovement()->StopMovementImmediately();
			Me->SpiderManAnim->DoubleJumpingDistClose = true;
			SetState(EState::IDLE);
			IdleState();
		}
	}
	else
	{
		if (dist <= 800.f)
		{
			//타겟이 보스일때만

			Me->GetCharacterMovement()->StopMovementImmediately();
			Me->SpiderManAnim->DoubleJumpingDistClose = true;
			SetState(EState::IDLE);
			IdleState();
		}
	}
			
	
	
	
}

void USpiderFSMComponent::TickAttack(const float& DeltaTime)
{
	// ECC_GameTraceChannel4 : 네거티브 채널
		// BossEnemy 를 향해 회전하도록 만들기

	FVector start = Me->GetActorLocation();
	FVector end = BossEnemy->GetActorLocation();
	
	FRotator rot = UKismetMathLibrary::FindLookAtRotation(start,end);

	FRotator interpRot = UKismetMathLibrary::RInterpTo_Constant(Me->GetActorRotation(),rot,DeltaTime,100.f);
	FRotator newRot = FRotator(0,interpRot.Yaw,0);
	Me->SetActorRotation(newRot);
	
}

void USpiderFSMComponent::TickSwing(const float& DeltaTime)
{
	
}

void USpiderFSMComponent::TickSwingAttack(const float& DeltaTime)
{
	
}

void USpiderFSMComponent::TickDamage(const float& DeltaTime)
{
}

void USpiderFSMComponent::TickDie(const float& DeltaTime)
{
}

void USpiderFSMComponent::SetState(EState NextState)
{
	//EState prevState = State; //기존의 값을 저장해두기
	FString mystate = UEnum::GetValueAsString(NextState);
	UE_LOG(LogTemp, Warning, TEXT("SetState : %s"), *mystate);
	State=NextState;
}

void USpiderFSMComponent::IdleState()
{
	//IdleState 가 될때 한번만 실행할 것들.
	Me->CableActor->CableComp->SetVisibility(false);
}


