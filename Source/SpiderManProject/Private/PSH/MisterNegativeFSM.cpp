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

	// Ÿ�� ����
	Target = Cast<ASpiderMan>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpiderMan::StaticClass()));
	// �ڽ� ����
	me = Cast<AMisterNegative>(GetOwner());
	if (me)
	{
		// �ִϸ��̼� ����
		MisterAnim = Cast<UMisterNegativeAnim>(me->GetMesh()->GetAnimInstance());
		DemonAnim = Cast<UDemonAnim>(me->Demon->GetAnimInstance());
	}

	// �ʿ� ���� �⺻ �������� (���� �ν��Ͻ��� ������� �ʱ� ������ ������ ������ ���� ���)
	FString mapName = UGameplayStatics::GetCurrentLevelName(GetWorld()); // ���� �� �̸� ��������
	if (mapName == "SpiderPhase1") // 1������ ���̸�
	{
		bisNextStage = false;
		curPage = 0;
	}
	else
	{
		bisNextStage = true;
	}

	curHp = maxHp; // �ִ�ü��
	stamina = 100; // �ִ� ���׹̳� ����
	bisGroggy = false;
	bisAttack = false;
	bisMaxPower = true;
	worldCenter = FVector(0, 0, 0);// �� �߾���ǥ ����
}

void UMisterNegativeFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (stamina <= 0) // ���׹̳� ������� ���� �׷α� ���� ����.
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

	// ������Ʈ ��ȭ Ȯ���� ���� �����
	const FString myState = UEnum::GetValueAsString(State);
	DrawDebugString(GetWorld(), me->GetActorLocation(), myState, nullptr, FColor::Red, 0, true);

	const FString myAnimState = UEnum::GetValueAsString(MisterAnim->AnimState);
	DrawDebugString(GetWorld(), GetOwner()->GetActorLocation() + FVector(0, 0, 50), myAnimState, nullptr, FColor::Yellow, 0, true);
	
}

void UMisterNegativeFSM::idleState()
{
	bisAttack = false;

	curTime += GetWorld()->DeltaTimeSeconds; // ������ Ÿ��
	 
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
	bisAttack = true; // �ǰ� �ִϸ��̼��� �ߵ� ����
	if (bisNextStage) // 2������ ���
	{
		RandomAttackCheak_2Page(); // 2������
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

void UMisterNegativeFSM::evasionState() // ȸ��
{
	me->SetUiVisble(false); // �׷α� UI ����
	SetLocation(Target->GetActorRightVector() * 150);
	SetState(EMisterNegativeState::Move); //  �̵�
}

void UMisterNegativeFSM::DamageState() // �¾�����
{
	// ���� ���¶� x
	// �׷α� ���¶� ����
	// �ƽ� �Ŀ� �ϋ� ���� �� ȸ�� ������Ʈ��.
	if (bisMaxPower) // �׷α� x ����
	{
		// 2�� ������� ȸ�� ������Ʈ��
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

	// damge = ���״�� ������ , MontageNum ��Ÿ�� �ִϸ��̼� Ȯ���� ���� , LaunchPower �о ��
	//  Dirction �о ���� 	LaunchPower = 1000; Dirction = (me->GetActorForwardVector() * -1);
void UMisterNegativeFSM::Dameged(float damge, int MontageNum , float LaunchPower , FVector Dirction)
{
	// ī�޶� �׽�Ʈ�� �ִϸ��̼�
	if (bisTest)
	{
		MisterAnim->RealDeadAnim();
		return;
	}
	curHp -= damge; // ü�¿� �������� ��

	if (curHp <= 0) // ü���� 0 ���ϸ�
	{
		curTime = 0; // �ð��� ���õ� ������ ���� �ʱ�ȭ
		if (bisNextStage) // 2���������� 0�̵Ǿ��ٸ�
		{
			me->SetUiVisble(false);
			MisterAnim->RealDeadAnim();
			DeadSpawnMonster();
		}
		else // 1���������� 0�̵Ǿ��ٸ�
		{
			me->SetUiVisble(false);
			MisterAnim->DeadAnim();
		}
		SetState(EMisterNegativeState::Die);
	}
	else // ü���� 0 ���� ũ�ٸ�
	{ 
		if (!bisNextStage) // ���� ���������� 1�����������
		{
			if (curHp <= maxHp / 2) // ü���� �����϶� ������ ��ȭ
			{
				curPage++;
			}
		}
		
		if (bisAttack)  // ���� �� �������� �ް� ������ ����.
		{
			return;
		}
		else   // ���� ���� �ƴ�
		{
			me->LaunchCharacter(Dirction * LaunchPower, false, false); // �ڷ� �˹�
			if (bisMaxPower) // �Ϲ� ���� 
			{
				hitcount++;
			}
			switch (MontageNum) // �ִϸ��̼� ���
			{
			case 1: //  ���� �׽�Ʈ�� ����Ʈ��ġ
				MisterAnim->RightHitAnim();
				break;
			case 2: //  ����Ʈ��ġ
				MisterAnim->LeftHitAnim();
				break;
			case 3: //  ��� ����Ʈ��ġ
				MisterAnim->HeadRightHitAnim();
				break;
			case 4: //  ��� ����Ʈ��ġ
				MisterAnim->HeadLefttHitAnim();
				break;
			case 5: //  ���� ��ġ
				MisterAnim->FrontHitAnim();
				break;
			}
			if (!bisGroggy) // �׷α� ���¿��� ����
			{
				SetState(EMisterNegativeState::Damage);
			}
			
		}
		
	}
}

void UMisterNegativeFSM::DieState() // ����
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

void UMisterNegativeFSM::GroggyState() // ����
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

void UMisterNegativeFSM::MoveState() // �߾����� �̵�
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
	// ���� ��ġ �κ��� �� �������� ���
	if (me->bisDemonAttack)
	{
		me->GetMesh()->SetRelativeLocation(me->GetMesh()->GetRelativeLocation() + me->GetActorUpVector() * 50 * GetWorld()->DeltaTimeSeconds); // ���̵�	
	}
	else
	{
		me->GetMesh()->SetRelativeLocation(me->GetMesh()->GetRelativeLocation() + (me->GetActorUpVector() *-1) * 50 * GetWorld()->DeltaTimeSeconds); // �Ʒ��̵�
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
		me->GetMesh()->SetRelativeLocation(me->GetMesh()->GetRelativeLocation() + me->GetActorUpVector() * 50 * GetWorld()->DeltaTimeSeconds); // ���̵�	
	}
	else
	{
		me->GetMesh()->SetRelativeLocation(me->GetMesh()->GetRelativeLocation() + (me->GetActorUpVector() * -1) * 50 * GetWorld()->DeltaTimeSeconds); // �Ʒ��̵�
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

	me->SetActorLocation(CurLoc); // �������̵�

	if (Alpha >= 1)
	{
		Alpha = 0;
		EndState(BeforState);
	}
	
}

void UMisterNegativeFSM::SetLocation(FVector TargetLocation)
{
	StartLoc = me->GetActorLocation(); // ������ġ
	TargetLoc = TargetLocation; // Ÿ���� ��ġ
	Dir = TargetLoc - StartLoc; // ����
	Dir.Normalize(); // ���� �ʱ�ȭ

// 	MeRotation = UKismetMathLibrary::MakeRotFromZX(me->GetActorUpVector(), Dir); // ��븦���� ȸ��
// 	me->SetActorRotation(MeRotation);

	dist = FVector::Dist(StartLoc, TargetLoc); // ���� ���� ��ġ ������ �Ÿ�
	EndLoc = StartLoc + Dir * dist; // ���� + ���� * ���� ��ġ������ �Ÿ�
	EndLoc.Z = StartLoc.Z; // ���δ� �̵� ���ϴ� ���� ��ġ�� ����
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
	// ���� ���� ��ȯ
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
		StartLoc = me->GetActorLocation(); // ������ġ
		TargetLoc = Target->GetActorLocation(); // Ÿ���� ��ġ
		Dir = TargetLoc - StartLoc; // ����
		Dir.Normalize(); // ���� �ʱ�ȭ
		me->SetDemonCollision(true);
		break;
	case EMisterNegativeState::DemonAttack2_idle:
		break;
	case EMisterNegativeState::DemonAttack2_Move:
		break;
	case EMisterNegativeState::DemonAttack2_Attack:
		StartLoc = me->GetActorLocation(); // ������ġ
		TargetLoc = Target->GetActorLocation(); // Ÿ���� ��ġ
		Dir = TargetLoc - StartLoc; // ����
		Dir.Normalize(); // ���� �ʱ�ȭ
		me->SetDemonCollision(true);
		break;
	}
}

// ��ȯ
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
