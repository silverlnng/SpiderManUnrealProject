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

	// ���� ������
	curPage = 2;
	stamina = 100;
}


// Called every frame
void UMisterNegativeFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (stamina <= 0)
	{
		SetState(EMisterNegativeState::Groggy);
	}

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
	case EMisterNegativeState::Groggy_loop:
		Groggy_loopState();
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
	case EMisterNegativeState::LightningAttack: // ����Ʈ�� ���ø� 
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
	if (curTime >= AttackDelayTime)
	{
		SetState(EMisterNegativeState::Attack);
		curTime = 0;
	}
}

void UMisterNegativeFSM::AttackState() 
{
	switch (curPage)
	{
		case 0:
		SetState(EMisterNegativeState::LightningstepAttack);
		UE_LOG(LogTemp, Warning, TEXT("curPage0"));
		break;
		case 1:
		RandomAttackCheak1();	
		UE_LOG(LogTemp, Warning, TEXT("curPage1"));
		break;
		case 2 :
		RandomAttackCheak2();	
		UE_LOG(LogTemp, Warning, TEXT("curPage2"));
		break;
	}
}

void UMisterNegativeFSM::evasionState() // ȸ��
{

}

void UMisterNegativeFSM::DamageState() // �¾�����
{
	
}

void UMisterNegativeFSM::GroggyState() // ����
{
	stamina = 100;
}

void UMisterNegativeFSM::Groggy_loopState()
{
	curTime += GetWorld()->DeltaTimeSeconds;
	UE_LOG(LogTemp, Warning, TEXT("Groggy_loopState"));
	if (curTime >= 5)
	{
		StartLoc = me->GetActorLocation();
		TargetLoc = FVector(0, 0, 0);
		Dir = TargetLoc - StartLoc; // Ÿ�ٿ� ����
		Dir.Normalize();

		MeRotation = FRotationMatrix::MakeFromXZ(Dir, Dir).Rotator();
		me->SetActorRotation(MeRotation);

		dist = FVector::Dist(StartLoc, TargetLoc); // ���� ���� ��ġ
		EndLoc = StartLoc + Dir * dist;
		EndLoc.Z = StartLoc.Z;
		SetState(EMisterNegativeState::Move);
		curTime = 0;
	}
}

void UMisterNegativeFSM::MoveState() // �߾����� �̵�
{
	UE_LOG(LogTemp, Warning, TEXT("Move"));
	Alpha += GetWorld()->DeltaTimeSeconds * 4;
	CurLoc = UKismetMathLibrary::VEase(StartLoc, EndLoc, Alpha, EEasingFunc::SinusoidalIn);

	me->SetActorLocation(CurLoc); // �������̵�

	if (Alpha >= 1)
	{
		SetState(EMisterNegativeState::Idle);
		Alpha = 0;
	}
}

void UMisterNegativeFSM::DieState() // ����
{

}

void UMisterNegativeFSM::RandomAttackCheak1()
{
	int RandemNum = FMath::RandRange(1, 3);

	switch (RandemNum)
	{
	case 1:
		SetState(EMisterNegativeState::LightningAttack);
		break;

	case 2:
		SetState(EMisterNegativeState::SpinAttack_idle);
		break;

	case 3:
		SetState(EMisterNegativeState::ChargingAttack_idle);
		break;

	default:
		break;
	}
}
void UMisterNegativeFSM::RandomAttackCheak2() // ���� ������ �� ���
{
	int RandemNum = FMath::RandRange(1, 3);
	switch (RandemNum)
	{
	case 1:
		SetState(EMisterNegativeState::LightningstepAttack); // ����
		break;

	case 2:
		SetState(EMisterNegativeState::SpinAttack_idle);
		break;

	case 3:
		SetState(EMisterNegativeState::ChargingAttack_idle);
		break;

	default:
		break;
	}
}

void UMisterNegativeFSM::LightningAttackState()
{
	
}


void UMisterNegativeFSM::LightningStepAttackState()
{
	// ����Ʈ�� ���� ����
}

void UMisterNegativeFSM::LightningstepAttack_IdleState()
{
	// idle ��븦 ���� ȸ��
	StartLoc = me->GetActorLocation();
	TargetLoc = Target->GetActorLocation();
	Dir = TargetLoc - StartLoc; // Ÿ�ٿ� ����
	Dir.Normalize();

	MeRotation = FRotationMatrix::MakeFromXZ(Dir, Dir).Rotator();
	me->SetActorRotation(MeRotation);

	dist = FVector::Dist(StartLoc, TargetLoc); // ���� ���� ��ġ
	EndLoc = StartLoc + Dir * dist;
	EndLoc.Z = StartLoc.Z;
}

void UMisterNegativeFSM::LightningStepAttackState_stepAttackState()
{
	Alpha += GetWorld()->DeltaTimeSeconds * 0.7f;
	CurLoc = UKismetMathLibrary::VEase(StartLoc, EndLoc, Alpha, EEasingFunc::SinusoidalIn);

 	me->SetActorLocation(CurLoc); // �������̵�

	if (Alpha >= 1.25f)
	{
		stamina -= 20;
		SetState(EMisterNegativeState::Idle);
		Alpha = 0;
	}
}


void UMisterNegativeFSM::SpinAttackState_IdleState()
{
	StartLoc = me->GetActorLocation();
	TargetLoc = Target->GetActorLocation();
	Dir = TargetLoc - StartLoc;
	Dir.Normalize();

	MeRotation = FRotationMatrix::MakeFromXZ(Dir, Dir).Rotator();
	me->SetActorRotation(MeRotation);

	dist = FVector::Dist(StartLoc, TargetLoc); // ���� ���� ��ġ
	EndLoc = StartLoc + Dir * dist;
	EndLoc.Z = StartLoc.Z;
}

void UMisterNegativeFSM::SpinAttackState_AttackState()
{
	Alpha += GetWorld()->DeltaTimeSeconds;
	CurLoc = UKismetMathLibrary::VEase(StartLoc, EndLoc, Alpha, EEasingFunc::SinusoidalIn);

	me->SetActorLocation(CurLoc); // �������̵�

	if (Alpha >= 1)
	{
		stamina -= 20;
		SetState(EMisterNegativeState::Idle);
		Alpha = 0;
	}
}

void UMisterNegativeFSM::ChargingAttack_IdleState()
{
	
	
}

void UMisterNegativeFSM::ChargingAttack_AttackState()
{
	
}

void UMisterNegativeFSM::SetState(EMisterNegativeState NewState)
{
	State = NewState;
	MisterAnim->AnimState = State;
}

void UMisterNegativeFSM::EndState(EMisterNegativeState endState) 
{
	UE_LOG(LogTemp, Warning, TEXT("AnimEnd"));
	switch (endState)
	{
	case EMisterNegativeState::Idle:
		break;
	case EMisterNegativeState::Move:
		break;
	case EMisterNegativeState::Damage:
		break;
	case EMisterNegativeState::Groggy:
		SetState(EMisterNegativeState::Groggy_loop);
		break;
	case EMisterNegativeState::Groggy_loop:
		break;
	case EMisterNegativeState::Attack:
		break;
	case EMisterNegativeState::evasion:
		break;
	case EMisterNegativeState::Die:
		break;
	case EMisterNegativeState::LightningAttack:
		stamina -= 20;
		SetState(EMisterNegativeState::Idle);
		break;
	case EMisterNegativeState::LightningstepAttack:
		stamina -= 10;
		SetState(EMisterNegativeState::LightningstepAttack_Idle);
		break;
	case EMisterNegativeState::LightningstepAttack_Idle:
		SetState(EMisterNegativeState::LightningstepAttack_Attack);
		break;
	case EMisterNegativeState::LightningstepAttack_Attack:
		break;
	case EMisterNegativeState::SpinAttack_idle:
		SetState(EMisterNegativeState::SpinAttack_Attack);
		break;
	case EMisterNegativeState::SpinAttack_Attack:
		break;
	case EMisterNegativeState::ChargingAttack_idle:
		SetState(EMisterNegativeState::ChargingAttack_Attack);
		break;
	case EMisterNegativeState::ChargingAttack_Attack:
		stamina -= 20;
		SetState(EMisterNegativeState::Idle);
		break;
	}
	// ���� ���¸� �� �� ������
	// ���� ���¿��� ���� ���·� �ѱ��.
}

