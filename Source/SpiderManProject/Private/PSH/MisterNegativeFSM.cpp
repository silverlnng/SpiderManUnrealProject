// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/MisterNegativeFSM.h"
#include "PSH/MisterNegative.h"
#include "PSH/MisterNegativeAnim.h"

// Sets default values for this component's properties
UMisterNegativeFSM::UMisterNegativeFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMisterNegativeFSM::BeginPlay()
{
	Super::BeginPlay();

	me = Cast<AMisterNegative>(GetOwner());
	if (me)
	{
		MisterAnim = Cast<UMisterNegativeAnim>(me->GetMesh()->GetAnimInstance());
	}

	
	
}


// Called every frame
void UMisterNegativeFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (State)
	{
	case EMisterNegativeState::Idle:
		idleState();
		break;
	case EMisterNegativeState::Move:
		MoveState();
		break;
	case EMisterNegativeState::Damage:
		DamageState();
		break;
	case EMisterNegativeState::Groggy: // 그로기
		GroggyState();
		break;
	case EMisterNegativeState::Attack:
		AttackState();

	case EMisterNegativeState::LightningAttack:
		LightningAttackState();
		break;
	
	case EMisterNegativeState::StepAttack:
		StepAttackState();
		break;
	
	case EMisterNegativeState::SpinAttack:
		SpinAttackState();
		break;
	
	case EMisterNegativeState::ChargingAttack:
		ChargingAttackState();
		break;
	
	case EMisterNegativeState::evasion: // 회피
		evasionState();
		break;
	case EMisterNegativeState::Die:
		DieState();
		break;
	default:
		break;
	}
}

void UMisterNegativeFSM::idleState()
{
	curTime += GetWorld()->DeltaTimeSeconds;

	if (curTime >= AttackDelayTime)
	{
		State = EMisterNegativeState::Attack;
		MisterAnim->AnimState = State;
		curTime = 0;
	}
}

void UMisterNegativeFSM::AttackState() 
{
	RandomAttackCheak();
}

void UMisterNegativeFSM::evasionState() // 회피
{

}

void UMisterNegativeFSM::DamageState() // 맞았을때
{

}

void UMisterNegativeFSM::GroggyState() // 스턴
{

}

void UMisterNegativeFSM::MoveState() // 중앙으로 이동
{

}

void UMisterNegativeFSM::DieState() // 죽음
{

}

void UMisterNegativeFSM::RandomAttackCheak()
{
	int RandemNum = FMath::RandRange(1,4);

	switch (RandemNum)
	{
		case 1:
			UE_LOG(LogTemp,Warning,TEXT("LightningAttack"));
			State = EMisterNegativeState::LightningAttack;
			MisterAnim->AnimState = State;
			break;

		case 2:
			UE_LOG(LogTemp, Warning, TEXT("StepAttack"));
			State = EMisterNegativeState::StepAttack;
			MisterAnim->AnimState = State;
			break;

		case 3:
			UE_LOG(LogTemp, Warning, TEXT("SpinAttack"));
			State = EMisterNegativeState::SpinAttack;
			MisterAnim->AnimState = State;
			break;

		case 4:
			UE_LOG(LogTemp, Warning, TEXT("ChargingAttack"));
			State = EMisterNegativeState::ChargingAttack;
			MisterAnim->AnimState = State;
			break;

		default:
			break;
	}
}

void UMisterNegativeFSM::LightningAttackState()
{
		UE_LOG(LogTemp, Warning, TEXT("LightningAttackState"));
}

void UMisterNegativeFSM::StepAttackState()
{
	UE_LOG(LogTemp, Warning, TEXT("StepAttackState"));
}

void UMisterNegativeFSM::SpinAttackState()
{
		UE_LOG(LogTemp, Warning, TEXT("SpinAttackState"));
}

void UMisterNegativeFSM::ChargingAttackState()
{
	switch (chargingAttackState)
	{
	case EMisterNegativeChargingAttackState::Idle:
		if (MisterAnim->bisNextAnim)
		{
			chargingAttackState = EMisterNegativeChargingAttackState::Attack;
			MisterAnim->chargingAttackAnimState = chargingAttackState;
			MisterAnim->bisNextAnim = false;
		}
		break;

	case EMisterNegativeChargingAttackState::Attack:
		if (MisterAnim->bisNextAnim)
		{
			State = EMisterNegativeState::Idle;
			MisterAnim->AnimState = State;
			chargingAttackState = EMisterNegativeChargingAttackState::Idle;
			MisterAnim->chargingAttackAnimState = chargingAttackState;
			MisterAnim->bisNextAnim = false;
		}
		break;
	}
		UE_LOG(LogTemp, Warning, TEXT("ChargingAttackState"));
}

