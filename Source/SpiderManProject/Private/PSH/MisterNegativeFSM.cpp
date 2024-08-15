// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/MisterNegativeFSM.h"
#include "PSH/MisterNegative.h"
#include "PSH/MisterNegativeAnim.h"
#include "YJ/SpiderMan.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "../SpiderManProjectCharacter.h"
#include "PSH/DemonAnim.h"
#include "Components/CapsuleComponent.h"
#include "PSH/SpawnMonster.h"


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

	// 타겟 설정
	Target = Cast<ASpiderMan>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpiderMan::StaticClass()));
	// 자신 설정
	me = Cast<AMisterNegative>(GetOwner());
	if (me)
	{
		// 애니메이션 설정
		MisterAnim = Cast<UMisterNegativeAnim>(me->GetMesh()->GetAnimInstance());
		DemonAnim = Cast<UDemonAnim>(me->Demon->GetAnimInstance());
	}

	// 맵에 따른 기본 스테이지 (게임 인스턴스를 사용하지 않기 때문에 페이즈 구별을 위해 사용)
	FString mapName = UGameplayStatics::GetCurrentLevelName(GetWorld()); // 현재 맵 이름 가져오기
	if (mapName == "SpiderPhase1") // 1페이지 맵이면
	{
		bisNextStage = false;
		curPage = 0;
	}
	else
	{
		bisNextStage = true;
	}

	curHp = maxHp; // 최대체력
	stamina = 100; // 최대 스테미나 설정
	bisGroggy = false;
	bisAttack = false;
	bisMaxPower = true;
	worldCenter = FVector(0, 0, 0);// 맵 중앙좌표 고정
}

void UMisterNegativeFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (stamina <= 0) // 스테미나 사용으로 인한 그로기 상태 돌입.
	{
		SetState(EMisterNegativeState::Groggy);
		stamina = 100;
	}

	switch (State)
	{
	case EMisterNegativeState::Idle: idleState();
		break;
	case EMisterNegativeState::Move: MoveState();
		break;
	case EMisterNegativeState::Damage: DamageState();
		break;
	case EMisterNegativeState::evasion: evasionState();
		break;
	case EMisterNegativeState::Die: DieState();
		break;
	case EMisterNegativeState::Groggy: GroggyState();
		break;
	case EMisterNegativeState::Groggy_loop: Groggy_loopState();
		break;
	case EMisterNegativeState::Groggy_end: Groggy_EndState();
		break;
	case EMisterNegativeState::Attack: AttackState();
		break;
	case EMisterNegativeState::LightningAttack: LightningAttackState();
		break;
	case EMisterNegativeState::stepAttack_Idle: stepAttack_IdleState();
		break;
	case EMisterNegativeState::stepAttack_Attack: StepAttackState_AttackState();
		break;
	case EMisterNegativeState::SpinAttack_idle: SpinAttackState_IdleState();
		break;
	case EMisterNegativeState::SpinAttack_Attack: SpinAttackState_AttackState();
		break;
	case EMisterNegativeState::ChargingAttack_idle: ChargingAttack_IdleState();
		break;
	case EMisterNegativeState::ChargingAttack_Attack: ChargingAttack_AttackState();
		break;
	case EMisterNegativeState::DemonAttack1_idle: DemonAttack1_idleState();
		break;
	case EMisterNegativeState::DemonAttack1_Move: DemonAttack1_MoveState();
		break;
	case EMisterNegativeState::DemonAttack1_Attack: DemonAttack1_AttackState();
		break;
	case EMisterNegativeState::DemonAttack2_idle: DemonAttack2_idleState();
		break;
	case EMisterNegativeState::DemonAttack2_Move: DemonAttack2_MoveState();
		break;
	case EMisterNegativeState::DemonAttack2_Attack: DemonAttack2_AttackState();
		break;
	}

	// 스테이트 변화 확인을 위한 디버깅
	const FString myState = UEnum::GetValueAsString(State);
	DrawDebugString(GetWorld(), me->GetActorLocation(), myState, nullptr, FColor::Red, 0, true);

	const FString myAnimState = UEnum::GetValueAsString(MisterAnim->AnimState);
	DrawDebugString(GetWorld(), GetOwner()->GetActorLocation() + FVector(0, 0, 50), myAnimState, nullptr, FColor::Yellow, 0, true);
	
}

void UMisterNegativeFSM::idleState()
{
	bisAttack = false;

	curTime += GetWorld()->DeltaTimeSeconds; // 딜레이 타임
	 
	TargetLoc = Target->GetActorLocation();
	Dir = TargetLoc - me->GetActorLocation();
	Dir.Normalize();

	MeRotation = UKismetMathLibrary::MakeRotFromZX(me->GetActorUpVector(),Dir);
	me->SetActorRotation(MeRotation);

	if (curTime >= AttackDelayTime)
	{
		AttackDelayTime = 0.5f;
		SetState(EMisterNegativeState::Attack);
		curTime = 0;
	}
}

void UMisterNegativeFSM::AttackState() 
{
	bisAttack = true; // 피격 애니메이션을 발동 안함
	if (bisNextStage) // 2페이지 라면
	{
		RandomAttackCheak_2Page(); // 2페이지
	}
	else
	{
		switch (curPage)
		{
		case 0:
			RandomAttackCheak_1Page();
			break;
		case 1:
			RandomAttackCheak_1To2Page();
			break;
		default:
			RandomAttackCheak_1To2Page();
			break;
		}
	}
}

void UMisterNegativeFSM::evasionState() // 회피
{
	me->SetUiVisble(false); // 그로기 UI 끄기
	SetLocation(Target->GetActorRightVector() * 150);
	SetState(EMisterNegativeState::Move); //  이동
}

void UMisterNegativeFSM::DamageState() // 맞았을때
{
	// 공격 상태때 x
	// 그로기 상태때 실행
	// 맥스 파워 일떄 실행 단 회피 스테이트로.
	if (bisMaxPower) // 그로기 x 상태
	{
		// 2번 맞을경우 회피 스테이트로
		if (hitcount >= 2)
		{
		 SetState(EMisterNegativeState::evasion);
		 hitcount = 0;
		}
		else
		{
		SetState(EMisterNegativeState::Idle);
		}
	}

	
}

	// damge = 말그대로 데미지 , MontageNum 몽타주 애니메이션 확인을 위해 , LaunchPower 밀어낼 힘
	//  Dirction 밀어낼 방향 	LaunchPower = 1000; Dirction = (me->GetActorForwardVector() * -1);
void UMisterNegativeFSM::Dameged(float damge, int MontageNum , float LaunchPower , FVector Dirction)
{
	// 카메라 테스트용 애니메이션
	if (bisTest)
	{
		MisterAnim->RealDeadAnim();
		return;
	}
	curHp -= damge; // 체력에 데미지를 줌

	if (curHp <= 0) // 체력이 0 이하면
	{
		curTime = 0; // 시간과 관련된 모든것을 위해 초기화
		if (bisNextStage) // 2페이지에서 0이되었다면
		{
			me->SetUiVisble(false);
			MisterAnim->RealDeadAnim();
			DeadSpawnMonster();
		}
		else // 1페이지에서 0이되었다면
		{
			me->SetUiVisble(false);
			MisterAnim->DeadAnim();
		}
		SetState(EMisterNegativeState::Die);
	}
	else // 체력이 0 보다 크다면
	{ 
		if (!bisNextStage) // 현재 스테이지가 1스테이지라면
		{
			if (curHp <= maxHp / 2) // 체력이 절반일때 페이지 변화
			{
				curPage++;
			}
		}
		
		if (bisAttack)  // 공격 중 데미지만 달고 반응이 없음.
		{
			return;
		}
		else   // 공격 중이 아님
		{
			me->LaunchCharacter(Dirction * LaunchPower, false, false); // 뒤로 넉백
			if (bisMaxPower) // 일반 상태 
			{
				hitcount++;
			}
			switch (MontageNum) // 애니메이션 재생
			{
			case 1: //  현재 테스트용 레프트펀치
				MisterAnim->RightHitAnim();
				break;
			case 2: //  라이트펀치
				MisterAnim->LeftHitAnim();
				break;
			case 3: //  헤드 레프트펀치
				MisterAnim->HeadRightHitAnim();
				break;
			case 4: //  헤드 라이트펀치
				MisterAnim->HeadLefttHitAnim();
				break;
			case 5: //  정면 펀치
				MisterAnim->FrontHitAnim();
				break;
			}
			if (!bisGroggy) // 그로기 상태에서 진입
			{
				SetState(EMisterNegativeState::Damage);
			}
			
		}
		
	}
}

void UMisterNegativeFSM::DieState() // 죽음
{
	if (bisNextStage)
	{
		curTime += GetWorld()->DeltaRealTimeSeconds;
		if (curTime >= 5)
		{
			me->Ending();
			curTime = 0;
			me->Destroy();
		}
	}
	else
	{
		return;
	}
}

void UMisterNegativeFSM::GroggyState() // 스턴
{
	bisAttack = false;
	bisGroggy = true;
	bisMaxPower = false;
	me->SetUiVisble(true);
}

void UMisterNegativeFSM::Groggy_loopState()
{
	curTime += GetWorld()->DeltaTimeSeconds;
	if (curTime >= GroggyTime)
	{
		SetState(EMisterNegativeState::Groggy_end);
		curTime = 0;
	}
}

void UMisterNegativeFSM::Groggy_EndState()
{
	SetLocation(worldCenter);
	bisGroggy = false;
	bisMaxPower = true;
}

void UMisterNegativeFSM::MoveState() // 중앙으로 이동
{
	GoForMove(GetWorld()->DeltaTimeSeconds*3,State);
}
void UMisterNegativeFSM::LightningAttackState()
{
	
}

void UMisterNegativeFSM::stepAttack_IdleState()
{
	SetLocation(Target->GetActorLocation());
}

void UMisterNegativeFSM::StepAttackState_AttackState()
{
	GoForMove(GetWorld()->DeltaTimeSeconds * 0.7f , State);
}


void UMisterNegativeFSM::SpinAttackState_IdleState()
{
	SetLocation(Target->GetActorLocation());
}

void UMisterNegativeFSM::SpinAttackState_AttackState()
{
	GoForMove(GetWorld()->DeltaTimeSeconds,State);
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
	if (curTime >= 1)
	{
		demonMeshLocation = me->GetMesh()->GetRelativeLocation();
		SetLocation(worldCenter);
		SetState(EMisterNegativeState::DemonAttack1_Move);
		curTime = 0;
	}
}

void UMisterNegativeFSM::DemonAttack1_MoveState()
{
	
	GoForMove(GetWorld()->DeltaTimeSeconds * 3, State);

}

void UMisterNegativeFSM::DemonAttack1_AttackState()
{
	// 현재 위치 로부터 위 방향으로 상승
	if (me->bisDemonAttack)
	{
		me->GetMesh()->SetRelativeLocation(me->GetMesh()->GetRelativeLocation() + me->GetActorUpVector() * 50 * GetWorld()->DeltaTimeSeconds); // 위이동	
	}
	else
	{
		me->GetMesh()->SetRelativeLocation(me->GetMesh()->GetRelativeLocation() + (me->GetActorUpVector() *-1) * 50 * GetWorld()->DeltaTimeSeconds); // 아래이동
	}
}

void UMisterNegativeFSM::DemonAttack2_idleState()
{
	curTime += GetWorld()->DeltaTimeSeconds;
	if (curTime >= 1)
	{
		demonMeshLocation = me->GetMesh()->GetRelativeLocation();
		SetLocation(worldCenter);
		SetState(EMisterNegativeState::DemonAttack2_Move);
		curTime = 0;
	}
}

void UMisterNegativeFSM::DemonAttack2_MoveState()
{
	GoForMove(GetWorld()->DeltaTimeSeconds * 3,State);
}

void UMisterNegativeFSM::DemonAttack2_AttackState()
{
	
	if (me->bisDemonAttack)
	{
		me->GetMesh()->SetRelativeLocation(me->GetMesh()->GetRelativeLocation() + me->GetActorUpVector() * 50 * GetWorld()->DeltaTimeSeconds); // 위이동	
	}
	else
	{
		me->GetMesh()->SetRelativeLocation(me->GetMesh()->GetRelativeLocation() + (me->GetActorUpVector() * -1) * 50 * GetWorld()->DeltaTimeSeconds); // 아래이동
	}
}

void UMisterNegativeFSM::RandomAttackCheak_1Page()
{
	int RandemNum = FMath::RandRange(1, 3);
	switch (RandemNum)
	{
	case 1:
		SetState(EMisterNegativeState::LightningAttack);
		break;
	case 2:
		SetState(EMisterNegativeState::stepAttack_Idle);
		break;
	}
}

void UMisterNegativeFSM::RandomAttackCheak_1To2Page()
{
	int RandemNum = FMath::RandRange(1, 4);
	switch (RandemNum)
	{
	case 1:
		SetState(EMisterNegativeState::LightningAttack);
		break;
	case 2:
		SetState(EMisterNegativeState::stepAttack_Idle);
		break;
	case 3:
		SetState(EMisterNegativeState::SpinAttack_idle);
		break;
	case 4:
		SetState(EMisterNegativeState::ChargingAttack_idle);
		break;
	}
}

void UMisterNegativeFSM::RandomAttackCheak_2Page()
{
	int RandemNum = FMath::RandRange(5, 6);
	switch (RandemNum)
	{
	case 1:
		SetState(EMisterNegativeState::LightningAttack);
		break;
	case 2:
		SetState(EMisterNegativeState::stepAttack_Idle);
		break;
	case 3:
		SetState(EMisterNegativeState::SpinAttack_idle);
		break;
	case 4:
		SetState(EMisterNegativeState::ChargingAttack_idle);
		break;
	case 5:
		SetState(EMisterNegativeState::DemonAttack1_idle);
		me->Demon->SetRelativeLocation(FVector(-1, -1.9f, -7));
		me->Demon->SetRelativeRotation(FRotator(0, 0, 10));
		break;
	case 6:
		SetState(EMisterNegativeState::DemonAttack2_idle);
		me->Demon->SetRelativeLocation(FVector(0, -1.9f, -6));
		me->Demon->SetRelativeRotation(FRotator(0, 0, -10));
		break;
	}
}

void UMisterNegativeFSM::GoForMove(float Time, EMisterNegativeState BeforState)
{

	Alpha += Time;
	CurLoc = UKismetMathLibrary::VEase(StartLoc, EndLoc, Alpha, EEasingFunc::SinusoidalIn);

	me->SetActorLocation(CurLoc); // 앞으로이동

	if (Alpha >= 1)
	{
		Alpha = 0;
		EndState(BeforState);
	}
	
}

void UMisterNegativeFSM::SetLocation(FVector TargetLocation)
{
	StartLoc = me->GetActorLocation(); // 시작위치
	TargetLoc = TargetLocation; // 타겟의 위치
	Dir = TargetLoc - StartLoc; // 방향
	Dir.Normalize(); // 방향 초기화

// 	MeRotation = UKismetMathLibrary::MakeRotFromZX(me->GetActorUpVector(), Dir); // 상대를향해 회전
// 	me->SetActorRotation(MeRotation);

	dist = FVector::Dist(StartLoc, TargetLoc); // 돌진 최종 위치 까지의 거리
	EndLoc = StartLoc + Dir * dist; // 시작 + 방향 * 최종 위치까지의 거리
	EndLoc.Z = StartLoc.Z; // 위로는 이동 안하니 시작 위치로 고정
}


void UMisterNegativeFSM::SetState(EMisterNegativeState NewState)
{
	State = NewState;
	MisterAnim->AnimState = State;;
}

void UMisterNegativeFSM::EndState(EMisterNegativeState endState) 
{
	me->SwordCol->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	me->SetDemonCollision(false);
	switch (endState)
	{
	case EMisterNegativeState::Idle: break;
	case EMisterNegativeState::Move: SetState(EMisterNegativeState::Idle);
		break;

	case EMisterNegativeState::Damage: 	
				
		break;
	case EMisterNegativeState::evasion:
		break;
	case EMisterNegativeState::Die:
		break;

	case EMisterNegativeState::Groggy: 
		SetState(EMisterNegativeState::Groggy_loop);
		break;
	case EMisterNegativeState::Groggy_loop:
		break;
	case EMisterNegativeState::Groggy_end:
		SetState(EMisterNegativeState::Move);
		bisGroggy = false;
		break;

	case EMisterNegativeState::Attack:
		break;

	case EMisterNegativeState::LightningAttack: SetState(EMisterNegativeState::Idle);
		bisAttack = false;
		me->SwordCol->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		stamina -= 10;
		break;

	case EMisterNegativeState::stepAttack_Idle: SetState(EMisterNegativeState::stepAttack_Attack);
		break;
	case EMisterNegativeState::stepAttack_Attack: SetState(EMisterNegativeState::Idle);
		bisAttack = false;
		me->SwordCol->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		stamina -= 20;
		break;

	case EMisterNegativeState::SpinAttack_idle: SetState(EMisterNegativeState::SpinAttack_Attack);
		break;

	case EMisterNegativeState::SpinAttack_Attack: SetState(EMisterNegativeState::Idle);
		bisAttack = false;
		me->SwordCol->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		stamina -= 20;
		break;

	case EMisterNegativeState::ChargingAttack_idle: SetState(EMisterNegativeState::ChargingAttack_Attack);
		break;
	case EMisterNegativeState::ChargingAttack_Attack:
		bisAttack = false;
		me->SwordCol->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		stamina -= 20;
		SetState(EMisterNegativeState::Idle);
		break;

	case EMisterNegativeState::DemonAttack1_idle:
		break;
	case EMisterNegativeState::DemonAttack1_Move:
		DemonAnim->AnimState = EMisterNegativeState::DemonAttack1_Attack;
		SetState(EMisterNegativeState::DemonAttack1_Attack);
		
		me->bisDemonAttack = true;
		break;
	case EMisterNegativeState::DemonAttack1_Attack:
		stamina -= 50;
		me->GetMesh()->SetRelativeLocation(demonMeshLocation);
		bisAttack = false;
		DemonAnim->AnimState = EMisterNegativeState::Idle;
		SetState(EMisterNegativeState::Idle);
		me->SetDemonCollision(false);
		break;

	case EMisterNegativeState::DemonAttack2_idle:
		break;
	case EMisterNegativeState::DemonAttack2_Move:
		DemonAnim->AnimState = EMisterNegativeState::DemonAttack2_Attack;
		SetState(EMisterNegativeState::DemonAttack2_Attack);
		me->bisDemonAttack = true;

		break;
	case EMisterNegativeState::DemonAttack2_Attack:
		stamina -= 50;
		me->GetMesh()->SetRelativeLocation(demonMeshLocation);
		bisAttack = false;
		DemonAnim->AnimState = EMisterNegativeState::Idle;
		me->SetDemonCollision(false);
		SetState(EMisterNegativeState::Idle);
		break;
	}
}

void UMisterNegativeFSM::StartState(EMisterNegativeState startState)
{	
	// 시작 시점 반환
	switch (State)
	{
	case EMisterNegativeState::Idle:
		break;
	case EMisterNegativeState::Move:
		break;

	case EMisterNegativeState::Damage:
		break;
	case EMisterNegativeState::evasion:
		break;
	case EMisterNegativeState::Die:
		break;

	case EMisterNegativeState::Groggy:
		break;
	case EMisterNegativeState::Groggy_loop:
		break;
	case EMisterNegativeState::Groggy_end:
		me->Explosion();
		me->SetUiVisble(false);
		break;

	case EMisterNegativeState::Attack:
		break;

	case EMisterNegativeState::LightningAttack:
		me->SwordCol->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	case EMisterNegativeState::SpinAttack_idle:
		me->SwordCol->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	case EMisterNegativeState::stepAttack_Idle:
		me->SwordCol->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	case EMisterNegativeState::stepAttack_Attack:
		me->SwordCol->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	case EMisterNegativeState::SpinAttack_Attack:
		me->SwordCol->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	case EMisterNegativeState::ChargingAttack_idle:
		break;
	case EMisterNegativeState::ChargingAttack_Attack:
		me->SwordCol->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	case EMisterNegativeState::DemonAttack1_idle:
		break;
	case EMisterNegativeState::DemonAttack1_Move:
		break;
	case EMisterNegativeState::DemonAttack1_Attack:
		StartLoc = me->GetActorLocation(); // 시작위치
		TargetLoc = Target->GetActorLocation(); // 타겟의 위치
		Dir = TargetLoc - StartLoc; // 방향
		Dir.Normalize(); // 방향 초기화
		me->SetDemonCollision(true);
		break;
	case EMisterNegativeState::DemonAttack2_idle:
		break;
	case EMisterNegativeState::DemonAttack2_Move:
		break;
	case EMisterNegativeState::DemonAttack2_Attack:
		StartLoc = me->GetActorLocation(); // 시작위치
		TargetLoc = Target->GetActorLocation(); // 타겟의 위치
		Dir = TargetLoc - StartLoc; // 방향
		Dir.Normalize(); // 방향 초기화
		me->SetDemonCollision(true);
		break;
	}
}

// 소환
void UMisterNegativeFSM::DeadSpawnMonster()
{
	for (int i = 0; i < Monsters.Num(); i++)
	{
		Monsters[i]->FSM->Die();
	}
}

void UMisterNegativeFSM::SetMonster(ASpawnMonster* monster)
{
	Monsters.Emplace(monster);
}

void UMisterNegativeFSM::DeleteMonster(ASpawnMonster* monster)
{
	Monsters.RemoveSingle(monster);
}
