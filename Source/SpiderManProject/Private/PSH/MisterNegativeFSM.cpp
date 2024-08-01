// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/MisterNegativeFSM.h"
#include "PSH/MisterNegative.h"
#include "PSH/MisterNegativeAnim.h"
#include "YJ/SpiderMan.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "../SpiderManProjectCharacter.h"
#include "PSH/DemonAnim.h"

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
	Target = Cast<ASpiderMan>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpiderMan::StaticClass()));
	me = Cast<AMisterNegative>(GetOwner());
	if (me)
	{
		MisterAnim = Cast<UMisterNegativeAnim>(me->GetMesh()->GetAnimInstance());
		DemonAnim = Cast<UDemonAnim>(me->Demon->GetAnimInstance());
	}

	// 현재 페이지
	if (bisNextStage) // Level 2에서 페이지 나누기 사용.
	{
		 curPage = 2;
	}
	else
	{
		curPage = 0;
	}
	
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
	case EMisterNegativeState::DemonAttack1_idle:
		DemonAttack1_idleState();
		break;
	case EMisterNegativeState::DemonAttack1_Move:
		DemonAttack1_MoveState();
		break;
	case EMisterNegativeState::DemonAttack1_Attack:
		DemonAttack1_AttackState();	
		break;
	case EMisterNegativeState::DemonAttack2_idle:
		DemonAttack2_idleState();
		break;
	case EMisterNegativeState::DemonAttack2_Move:
		DemonAttack2_MoveState();
		break;
	case EMisterNegativeState::DemonAttack2_Attack:
		DemonAttack2_AttackState();
		break;
	}

}

void UMisterNegativeFSM::idleState()
{
	curTime += GetWorld()->DeltaTimeSeconds;
	TargetLoc = Target->GetActorLocation();
	Dir = TargetLoc - me->GetActorLocation();
	Dir.Normalize();

	MeRotation = UKismetMathLibrary::FindLookAtRotation(me->GetActorLocation(), Target->GetActorLocation());
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
		default:
		RandomAttackCheak2();
		UE_LOG(LogTemp, Warning, TEXT("defaultPage"));
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

void UMisterNegativeFSM::Groggy_loopState()
{
	curTime += GetWorld()->DeltaTimeSeconds;
	UE_LOG(LogTemp, Warning, TEXT("Groggy_loopState"));
	if (curTime >= 5)
	{
		StartLoc = me->GetActorLocation();
		TargetLoc = FVector(0, 0, 0);
		Dir = TargetLoc - StartLoc; // 타겟에 방향
		Dir.Normalize();

		MeRotation = UKismetMathLibrary::FindLookAtRotation(me->GetActorLocation(), Target->GetActorLocation());
		me->SetActorRotation(MeRotation);

		dist = FVector::Dist(StartLoc, TargetLoc); // 돌진 최종 위치
		EndLoc = StartLoc + Dir * dist;
		EndLoc.Z = StartLoc.Z;
		SetState(EMisterNegativeState::Move);
		curTime = 0;
	}
}

void UMisterNegativeFSM::MoveState() // 중앙으로 이동
{
	UE_LOG(LogTemp, Warning, TEXT("Move"));
	Alpha += GetWorld()->DeltaTimeSeconds * 4;
	CurLoc = UKismetMathLibrary::VEase(StartLoc, EndLoc, Alpha, EEasingFunc::SinusoidalIn);

	me->SetActorLocation(CurLoc); // 앞으로이동

	if (Alpha >= 1)
	{
		SetState(EMisterNegativeState::Idle);
		Alpha = 0;
	}
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
void UMisterNegativeFSM::RandomAttackCheak2() // 데몬 페이즈 때 사용
{
	int RandemNum = FMath::RandRange(1, 5);
	switch (RandemNum)
	{
	case 1:
		SetState(EMisterNegativeState::LightningstepAttack); // 종료
		break;

	case 2:
		SetState(EMisterNegativeState::SpinAttack_idle);
		break;

	case 3:
		SetState(EMisterNegativeState::ChargingAttack_idle);
		break;
	case 4:
		SetState(EMisterNegativeState::DemonAttack1_idle);
		me->Demon->SetRelativeLocation(FVector(-100, -190, -760));
		break;
	case 5:
		SetState(EMisterNegativeState::DemonAttack2_idle);
		me->Demon->SetRelativeLocation(FVector(-100, -190, -200));
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
	// 라이트닝 스탭 공격
}

void UMisterNegativeFSM::LightningstepAttack_IdleState()
{
	// idle 상대를 향해 회전
	StartLoc = me->GetActorLocation();
	TargetLoc = Target->GetActorLocation();
	Dir = TargetLoc - StartLoc; // 타겟에 방향
	Dir.Normalize();

	MeRotation = UKismetMathLibrary::FindLookAtRotation(me->GetActorLocation(), Target->GetActorLocation());
	me->SetActorRotation(MeRotation);

	dist = FVector::Dist(StartLoc, TargetLoc); // 돌진 최종 위치
	EndLoc = StartLoc + Dir * dist;
	EndLoc.Z = StartLoc.Z;
}

void UMisterNegativeFSM::LightningStepAttackState_stepAttackState()
{
	Alpha += GetWorld()->DeltaTimeSeconds * 0.7f;
	CurLoc = UKismetMathLibrary::VEase(StartLoc, EndLoc, Alpha, EEasingFunc::SinusoidalIn);

 	me->SetActorLocation(CurLoc); // 앞으로이동

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

	MeRotation = UKismetMathLibrary::FindLookAtRotation(me->GetActorLocation(), Target->GetActorLocation());
	me->SetActorRotation(MeRotation);

	dist = FVector::Dist(StartLoc, TargetLoc); // 돌진 최종 위치
	EndLoc = StartLoc + Dir * dist;
	EndLoc.Z = StartLoc.Z;
}

void UMisterNegativeFSM::SpinAttackState_AttackState()
{
	Alpha += GetWorld()->DeltaTimeSeconds;
	CurLoc = UKismetMathLibrary::VEase(StartLoc, EndLoc, Alpha, EEasingFunc::SinusoidalIn);

	me->SetActorLocation(CurLoc); // 앞으로이동

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

void UMisterNegativeFSM::DemonAttack1_idleState()
{
	curTime += GetWorld()->DeltaTimeSeconds;
	UE_LOG(LogTemp, Warning, TEXT("DemonAttack1_idleState"));
	if (curTime >= 1)
	{
	StartLoc = me->GetActorLocation();
	TargetLoc = FVector(0, 0, 0);
	Dir = TargetLoc - StartLoc; // 타겟에 방향
	Dir.Normalize();

	MeRotation = UKismetMathLibrary::FindLookAtRotation(me->GetActorLocation(), Target->GetActorLocation());
	me->SetActorRotation(MeRotation);

	dist = FVector::Dist(StartLoc, TargetLoc); // 돌진 최종 위치
	EndLoc = StartLoc + Dir * dist;
	EndLoc.Z = StartLoc.Z;
	SetState(EMisterNegativeState::DemonAttack1_Move);
	curTime = 0;
	}
}

void UMisterNegativeFSM::DemonAttack1_MoveState()
{
	UE_LOG(LogTemp, Warning, TEXT("DemonAttack1_MoveState"));
	Alpha += GetWorld()->DeltaTimeSeconds * 4;
	CurLoc = UKismetMathLibrary::VEase(StartLoc, EndLoc, Alpha, EEasingFunc::SinusoidalIn);

	me->SetActorLocation(CurLoc); // 앞으로이동

	if (Alpha >= 1)
	{
		MeRotation = UKismetMathLibrary::FindLookAtRotation(me->GetActorLocation(), Target->GetActorLocation());
		me->SetActorRotation(MeRotation);
		me->SetMeshVisible(true);
		SetState(EMisterNegativeState::DemonAttack1_Attack);
		DemonAnim->AnimState = EMisterNegativeState::DemonAttack1_Attack;
		Alpha = 0;
		demonMeshLocation = me->GetMesh()->GetRelativeLocation(); // 현재위치지정
		me->bisDemonAttack = true;
	}
}

void UMisterNegativeFSM::DemonAttack1_AttackState()
{
	UE_LOG(LogTemp, Warning, TEXT("DemonAttack1_MoveState"));
	
	// 현재 위치 로부터 위 방향으로 상승
	if (me->bisDemonAttack)
	{
		me->GetMesh()->SetRelativeLocation(demonMeshLocation + me->GetActorUpVector() * 500 * GetWorld()->DeltaTimeSeconds); // 위이동	
	}
	else
	{
		me->GetMesh()->SetRelativeLocation(demonMeshLocation + (me->GetActorUpVector() *-1) * 500 * GetWorld()->DeltaTimeSeconds); // 아래이동
	}

	// 노티파이 실행 시 내려가기 실시..?
	// 불값에 따라 올라가고 내려가기 ? V
}

void UMisterNegativeFSM::DemonAttack2_idleState()
{
	curTime += GetWorld()->DeltaTimeSeconds;
	UE_LOG(LogTemp, Warning, TEXT("DemonAttack2_idleState"));
	if (curTime >= 1)
	{
		StartLoc = me->GetActorLocation();
		TargetLoc = FVector(0, 0, 0);
		Dir = TargetLoc - StartLoc; // 타겟에 방향
		Dir.Normalize();

		MeRotation = UKismetMathLibrary::FindLookAtRotation(me->GetActorLocation(), Target->GetActorLocation());
		me->SetActorRotation(MeRotation);

		dist = FVector::Dist(StartLoc, TargetLoc); // 돌진 최종 위치
		EndLoc = StartLoc + Dir * dist;
		EndLoc.Z = StartLoc.Z;
		SetState(EMisterNegativeState::DemonAttack2_Move);
		curTime = 0;
	}
}

void UMisterNegativeFSM::DemonAttack2_MoveState()
{
	UE_LOG(LogTemp, Warning, TEXT("DemonAttack2_MoveState"));
	Alpha += GetWorld()->DeltaTimeSeconds * 4;
	CurLoc = UKismetMathLibrary::VEase(StartLoc, EndLoc, Alpha, EEasingFunc::SinusoidalIn);

	me->SetActorLocation(CurLoc); // 앞으로이동

	if (Alpha >= 1)
	{
		MeRotation = UKismetMathLibrary::FindLookAtRotation(me->GetActorLocation(),Target->GetActorLocation());
		me->SetActorRotation(MeRotation);
		me->SetMeshVisible(true);
		DemonAnim->AnimState = EMisterNegativeState::DemonAttack2_Attack;
		SetState(EMisterNegativeState::DemonAttack2_Attack);
		Alpha = 0;
		demonMeshLocation = me->GetMesh()->GetRelativeLocation();
		me->bisDemonAttack = true;
	}
}

void UMisterNegativeFSM::DemonAttack2_AttackState()
{
	if (me->bisDemonAttack)
	{
		me->GetMesh()->SetRelativeLocation(demonMeshLocation + me->GetActorUpVector() * 500 * GetWorld()->DeltaTimeSeconds); // 위이동	
	}
	else
	{
		me->GetMesh()->SetRelativeLocation(demonMeshLocation + (me->GetActorUpVector() * -1) * 500 * GetWorld()->DeltaTimeSeconds); // 아래이동
	}
}

void UMisterNegativeFSM::SetState(EMisterNegativeState NewState)
{
	State = NewState;
	MisterAnim->AnimState = State;;

}

void UMisterNegativeFSM::Dameged(float damge)
{
	curHp -= damge;
	UE_LOG(LogTemp,Warning,TEXT("Negative Damaged!!!!!!"));
	if (bisNextStage) // Level 2에서 페이지 나누기 사용.
	{
		
	}
	else
	{
		if (curHp <= maxHp / 2) // Level 1에서 페이지 나누기 사용
		{
			curPage++;
		}
	}
	

	if (curHp <= 0)
	{
		SetState(EMisterNegativeState::Die);
	}
	else
	{
		SetState(EMisterNegativeState::Damage);
	}
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
		stamina = 100;
		curPage++; // 임시 
		UE_LOG(LogTemp, Warning, TEXT("GroggyState"));
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
	case EMisterNegativeState::DemonAttack1_idle:
		break;
	case EMisterNegativeState::DemonAttack1_Move:
		break;
	case EMisterNegativeState::DemonAttack1_Attack:
		stamina -= 40;
		DemonAnim->AnimState = EMisterNegativeState::Idle;
		me->GetMesh()->SetRelativeLocation(demonMeshLocation);
		me->SetMeshVisible(false);
		SetState(EMisterNegativeState::Idle);
		break;
	case EMisterNegativeState::DemonAttack2_idle:
		break;
	case EMisterNegativeState::DemonAttack2_Move:
		break;
	case EMisterNegativeState::DemonAttack2_Attack:
		stamina -= 40;
		me->GetMesh()->SetRelativeLocation(demonMeshLocation);
		me->SetMeshVisible(false);
		DemonAnim->AnimState = EMisterNegativeState::Idle;
		SetState(EMisterNegativeState::Idle);
		break;
	}
	// 끝난 상태를 알 수 있으니
	// 끝난 상태에서 다음 상태로 넘긴다.
}

