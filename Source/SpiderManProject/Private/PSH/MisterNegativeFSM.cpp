// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/MisterNegativeFSM.h"
#include "PSH/MisterNegative.h"
#include "PSH/MisterNegativeAnim.h"
#include "YJ/SpiderMan.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "../SpiderManProjectCharacter.h"

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
	Target = Cast<ASpiderManProjectCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpiderManProjectCharacter::StaticClass()));
	me = Cast<AMisterNegative>(GetOwner());
	if (me)
	{
		MisterAnim = Cast<UMisterNegativeAnim>(me->GetMesh()->GetAnimInstance());
	}

	// 현재 페이지
	curPage = 0;
	
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
	case EMisterNegativeState::Groggy:
		GroggyState();
		break;
	case EMisterNegativeState::Attack:
		AttackState();
		break;
	case EMisterNegativeState::evasion:
		evasionState();
		break;
	case EMisterNegativeState::Die:
		DieState();
		break;
	case EMisterNegativeState::LightningAttack: // 라이트닝 어택만 
		LightningAttackState();
		break;
	case EMisterNegativeState::LightningstepAttack:
		LightningStepAttackState();
		break;
	case EMisterNegativeState::LightningstepAttack_Idle:
		LightningstepAttack_IdleState();
		break;
	case EMisterNegativeState::LightningstepAttack_Attack:
		LightningStepAttackState_stepAttackState();
		break;
	case EMisterNegativeState::SpinAttack_idle:
		SpinAttackState_IdleState();
		break;
	case EMisterNegativeState::SpinAttack_Attack:
		SpinAttackState_AttackState();
		break;
	case EMisterNegativeState::ChargingAttack_idle:
		ChargingAttack_IdleState();
		break;
	case EMisterNegativeState::ChargingAttack_Attack:
		ChargingAttack_AttackState();
		break;
	default:
		break;
	}
}

void UMisterNegativeFSM::idleState()
{
	curTime += GetWorld()->DeltaTimeSeconds;
	TargetLoc = Target->GetActorLocation();
	Dir = TargetLoc - me->GetActorLocation();
	Dir.Normalize();

	MeRotation = FRotationMatrix::MakeFromXZ(Dir, Dir).Rotator();
	me->SetActorRotation(MeRotation);
	UE_LOG(LogTemp, Warning, TEXT("idle"));
	if (curTime >= AttackDelayTime)
	{
		State = EMisterNegativeState::Attack;
		MisterAnim->AnimState = State;
		curTime = 0;
	}
}

void UMisterNegativeFSM::AttackState() 
{
	switch (curPage)
	{
		case 1:
		State = EMisterNegativeState::LightningstepAttack;
		MisterAnim->AnimState = State;
		UE_LOG(LogTemp, Warning, TEXT("AttackState"));
		break;
		case 2:
		RandomAttackCheak1();	
		break;
		case 0 :
		RandomAttackCheak2();	
		break;
	}
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

void UMisterNegativeFSM::RandomAttackCheak1()
{

	int RandemNum = FMath::RandRange(1, 3);

	switch (RandemNum)
	{
	case 1:
		UE_LOG(LogTemp, Warning, TEXT("LightningAttack"));
		State = EMisterNegativeState::LightningAttack;
		MisterAnim->AnimState = State;
		break;

	case 2:
		UE_LOG(LogTemp, Warning, TEXT("SpinAttack"));
		State = EMisterNegativeState::SpinAttack_idle;
		MisterAnim->AnimState = State;
		break;

	case 3:
		UE_LOG(LogTemp, Warning, TEXT("ChargingAttack"));
		State = EMisterNegativeState::ChargingAttack_idle;
		MisterAnim->AnimState = State;
		break;

	default:
		break;
	}
}
void UMisterNegativeFSM::RandomAttackCheak2() // 데몬 페이즈 때 사용
{
	int RandemNum = FMath::RandRange(1, 2);

	switch (RandemNum)
	{
	case 1:
		UE_LOG(LogTemp, Warning, TEXT("LightningAttack"));
		State = EMisterNegativeState::LightningstepAttack;
		MisterAnim->AnimState = State;
		break;

	case 3:
		UE_LOG(LogTemp, Warning, TEXT("SpinAttack"));
		State = EMisterNegativeState::SpinAttack_idle;
		MisterAnim->AnimState = State;
		break;

	case 4:
		UE_LOG(LogTemp, Warning, TEXT("ChargingAttack"));
		State = EMisterNegativeState::ChargingAttack_idle;
		MisterAnim->AnimState = State;
		break;

	default:
		break;
	}
}

void UMisterNegativeFSM::LightningAttackState()
{
	SetState(EMisterNegativeState::Idle);
}


void UMisterNegativeFSM::LightningStepAttackState()
{
	
	// 라이트닝 스탭 공격
}

void UMisterNegativeFSM::LightningstepAttack_IdleState()
{
	// idle 상대를 향해 회전
	MeRotation = FRotationMatrix::MakeFromXZ(Dir, Dir).Rotator();
	me->SetActorRotation(MeRotation);

	StartLoc = me->GetActorLocation();
	TargetLoc = Target->GetActorLocation();
	Dir = TargetLoc - StartLoc; // 타겟에 방향
	Dir.Normalize();

	dist = FVector::Dist(StartLoc, TargetLoc); // 돌진 최종 위치
	EndLoc = StartLoc + Dir * dist;
	EndLoc.Z = StartLoc.Z;
}

void UMisterNegativeFSM::LightningStepAttackState_stepAttackState()
{


	Alpha += GetWorld()->DeltaTimeSeconds*0.60f;
	UE_LOG(LogTemp, Warning, TEXT("%f"), Alpha);
	CurLoc = UKismetMathLibrary::VEase(StartLoc, EndLoc, Alpha, EEasingFunc::SinusoidalIn);


 	me->SetActorLocation(CurLoc);

	if (Alpha >= 1.25f)
	{
		SetState(EMisterNegativeState::Idle);
		Alpha = 0;
	}
	
}


void UMisterNegativeFSM::SpinAttackState_IdleState()
{
	SetState(EMisterNegativeState::SpinAttack_Attack);
}

void UMisterNegativeFSM::SpinAttackState_AttackState()
{

}

void UMisterNegativeFSM::ChargingAttack_IdleState()
{
	UE_LOG(LogTemp, Warning, TEXT("ChargingAttackState"));
	SetState(EMisterNegativeState::ChargingAttack_Attack);
}

void UMisterNegativeFSM::ChargingAttack_AttackState()
{
	SetState(EMisterNegativeState::Idle);
}

void UMisterNegativeFSM::SetState(EMisterNegativeState NewState)
{
	State = NewState;
	MisterAnim->AnimState = State;
}

void UMisterNegativeFSM::EndState(EMisterNegativeState endState)
{

	switch (endState)
	{
	case EMisterNegativeState::Idle:
		break;
	case EMisterNegativeState::Move:
		break;
	case EMisterNegativeState::Damage:
		break;
	case EMisterNegativeState::Groggy:
		break;
	case EMisterNegativeState::Attack:
		break;
	case EMisterNegativeState::evasion:
		break;
	case EMisterNegativeState::Die:
		break;
	case EMisterNegativeState::LightningAttack:
		break;
	case EMisterNegativeState::LightningstepAttack:
		SetState(EMisterNegativeState::LightningstepAttack_Idle);
		break;
	case EMisterNegativeState::LightningstepAttack_Idle:
		SetState(EMisterNegativeState::LightningstepAttack_Attack);
		break;
	case EMisterNegativeState::LightningstepAttack_Attack:
		SetState(EMisterNegativeState::Idle);
		break;
	case EMisterNegativeState::SpinAttack_idle:
		SetState(EMisterNegativeState::SpinAttack_Attack);
		break;
	case EMisterNegativeState::SpinAttack_Attack:
		SetState(EMisterNegativeState::Idle);
		break;
	case EMisterNegativeState::ChargingAttack_idle:
		SetState(EMisterNegativeState::ChargingAttack_Attack);
		break;
	case EMisterNegativeState::ChargingAttack_Attack:
		SetState(EMisterNegativeState::Idle);
		break;
	}
	// 끝난 상태를 알 수 있으니
	// 끝난 상태에서 다음 상태로 넘긴다.
}

