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

	FString mapName = UGameplayStatics::GetCurrentLevelName(GetWorld()); // ���� �� �̸� ��������
	if (mapName == "MisterMap") // 1������ ���̸�
	{
		bisNextStage = false;
	}
	else
	{
		bisNextStage = true;
	}

	curHp = maxHp;
	
	
	if (!bisNextStage) // 2�������� �ƴ϶��
	{
		curPage = 0;; // 1������
	}
	
	
	stamina = 100;

	worldCenter = FVector(0, 0, 0);// �� �߾���ǥ ����
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
	if (bisNextStage)
	{
		RandomAttackCheak2(); // 2������
	}
	else
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
		default:
			RandomAttackCheak1();
			UE_LOG(LogTemp, Warning, TEXT("defaultPage"));
			break;
		}
	}
	
}

void UMisterNegativeFSM::evasionState() // ȸ��
{
	// �׽�Ʈ�� �׳� �̵�
	StartLoc = me->GetActorLocation();
	TargetLoc = worldCenter; // ���� ����� Ÿ������ ����
	Dir = TargetLoc - StartLoc; // ���� �� ��� ����
	Dir.Normalize();

	dist = FVector::Dist(StartLoc, TargetLoc); // ���� ���� ��ġ �Ÿ�
	EndLoc = StartLoc + Dir * dist;  // ���� ���� ��ġ
	EndLoc.Z = StartLoc.Z; // �� �������δ� �̵����� �ʱ⶧���� ���� ��ġ�� ����.
	SetState(EMisterNegativeState::Move); // ������� �̵�
	curTime = 0;
}

void UMisterNegativeFSM::DamageState() // �¾�����
{
	hitcount++;
	curTime += GetWorld()->DeltaTimeSeconds;
	UE_LOG(LogTemp, Warning, TEXT("DamageState"));
	if (bisMaxPowerMode) // �׷α� x ����
	{
		// 2�� ������� ȸ�� ������Ʈ��
		if(hitcount >=2)
		SetState(EMisterNegativeState::evasion);
	}
	else // �׷α� ������Ʈ ����
	{
		// �ð��� ������ move������Ʈ��
		if (curTime >= 5)
		{
			StartLoc = me->GetActorLocation();
			TargetLoc = worldCenter; // ���� ����� Ÿ������ ����
			Dir = TargetLoc - StartLoc; // ���� �� ��� ����
			Dir.Normalize();

			dist = FVector::Dist(StartLoc, TargetLoc); // ���� ���� ��ġ �Ÿ�
			EndLoc = StartLoc + Dir * dist;  // ���� ���� ��ġ
			EndLoc.Z = StartLoc.Z; // �� �������δ� �̵����� �ʱ⶧���� ���� ��ġ�� ����.
			SetState(EMisterNegativeState::Move); // ������� �̵�
			curTime = 0;
		}
	}
}

void UMisterNegativeFSM::GroggyState() // ����
{
	bisMaxPowerMode = false;
}

void UMisterNegativeFSM::Groggy_loopState()
{
	curTime += GetWorld()->DeltaTimeSeconds;
	if (curTime >= 5)
	{
		StartLoc = me->GetActorLocation();
		TargetLoc = worldCenter; // ���� ����� Ÿ������ ����
		Dir = TargetLoc - StartLoc; // Ÿ�ٿ� ����
		Dir.Normalize();

		dist = FVector::Dist(StartLoc, TargetLoc); // ���� ���� ��ġ
		EndLoc = StartLoc + Dir * dist;
		EndLoc.Z = StartLoc.Z;
		SetState(EMisterNegativeState::Move);
		bisMaxPowerMode = true;
		curTime = 0;
	}
}

void UMisterNegativeFSM::MoveState() // �߾����� �̵�
{
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
	if (bisNextStage)
	{
		me->Destroy();
	}
	else
	{
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("MisterMap_page2"));// �������� ����
	}
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
	int RandemNum = FMath::RandRange(1, 2);
	switch (RandemNum)
	{
// 	case 1:
// 		SetState(EMisterNegativeState::LightningstepAttack); // ����
// 		break;
// 
// 	case 2:
// 		SetState(EMisterNegativeState::SpinAttack_idle);
// 		break;
// 
// 	case 3:
// 		SetState(EMisterNegativeState::ChargingAttack_idle);
// 		break;
	case 1:
		SetState(EMisterNegativeState::DemonAttack1_idle);
		me->Demon->SetRelativeLocation(FVector(-100, -190, -760));
		me->Demon->SetRelativeRotation(FRotator(0, 0, 10));

		break;
	case 2:
		SetState(EMisterNegativeState::DemonAttack2_idle);
		me->Demon->SetRelativeLocation(FVector(0, -190, -200));
		me->Demon->SetRelativeRotation(FRotator(0, 0, 0));
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

	MeRotation = UKismetMathLibrary::FindLookAtRotation(me->GetActorLocation(), Target->GetActorLocation());
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
		me->SwordCol->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
		me->SwordCol->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
	if (curTime >= 1)
	{
	StartLoc = me->GetActorLocation();
	TargetLoc = worldCenter; // ���� ����� ��ǥ�� ����
	Dir = TargetLoc - StartLoc; // ��ǥ�� ����
	Dir.Normalize();

	MeRotation = UKismetMathLibrary::FindLookAtRotation(me->GetActorLocation(), Target->GetActorLocation());
	me->SetActorRotation(MeRotation);

	dist = FVector::Dist(StartLoc, TargetLoc); // ���� ���� ��ġ
	EndLoc = StartLoc + Dir * dist;
	EndLoc.Z = StartLoc.Z;
	SetState(EMisterNegativeState::DemonAttack1_Move);
	curTime = 0;
	}
}

void UMisterNegativeFSM::DemonAttack1_MoveState()
{
	Alpha += GetWorld()->DeltaTimeSeconds * 4;
	CurLoc = UKismetMathLibrary::VEase(StartLoc, EndLoc, Alpha, EEasingFunc::SinusoidalIn);

	me->SetActorLocation(CurLoc); // �������̵�

	if (Alpha >= 1)
	{
		MeRotation = UKismetMathLibrary::FindLookAtRotation(me->GetActorLocation(), Target->GetActorLocation());
		me->SetActorRotation(MeRotation);
		me->SetMeshVisible(true);
		SetState(EMisterNegativeState::DemonAttack1_Attack);
		DemonAnim->AnimState = EMisterNegativeState::DemonAttack1_Attack;
		Alpha = 0;
		demonMeshLocation = me->GetMesh()->GetRelativeLocation(); // ������ġ����
		me->bisDemonAttack = true;
	}
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

	// ��Ƽ���� ���� �� �������� �ǽ�..?
	// �Ұ��� ���� �ö󰡰� �������� ? V
}

void UMisterNegativeFSM::DemonAttack2_idleState()
{
	curTime += GetWorld()->DeltaTimeSeconds;
	if (curTime >= 1)
	{
		StartLoc = me->GetActorLocation();
		TargetLoc = worldCenter; // ���� �߾��� Ÿ������ ����
		Dir = TargetLoc - StartLoc; // Ÿ�ٿ� ����
		Dir.Normalize();

		MeRotation = UKismetMathLibrary::FindLookAtRotation(me->GetActorLocation(), Target->GetActorLocation());
		me->SetActorRotation(MeRotation);

		dist = FVector::Dist(StartLoc, TargetLoc); // ���� ���� ��ġ
		EndLoc = StartLoc + Dir * dist;
		EndLoc.Z = StartLoc.Z;
		SetState(EMisterNegativeState::DemonAttack2_Move);
		curTime = 0;
	}
}

void UMisterNegativeFSM::DemonAttack2_MoveState()
{
	Alpha += GetWorld()->DeltaTimeSeconds * 4;
	CurLoc = UKismetMathLibrary::VEase(StartLoc, EndLoc, Alpha, EEasingFunc::SinusoidalIn);

	me->SetActorLocation(CurLoc); // �������̵�

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
		me->GetMesh()->SetRelativeLocation(me->GetMesh()->GetRelativeLocation() + me->GetActorUpVector() * 50 * GetWorld()->DeltaTimeSeconds); // ���̵�	
	}
	else
	{
		me->GetMesh()->SetRelativeLocation(me->GetMesh()->GetRelativeLocation() + (me->GetActorUpVector() * -1) * 50 * GetWorld()->DeltaTimeSeconds); // �Ʒ��̵�
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
	if (bisNextStage) // Level 2���� ������ ������ ���.
	{
		
	}
	else
	{
		if (curHp <= maxHp / 2) // Level 1���� ������ ������ ���
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
	me->SwordCol->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	me->demonCol->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
		stamina -= 50;
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
		stamina -= 50;
		me->GetMesh()->SetRelativeLocation(demonMeshLocation);
		me->SetMeshVisible(false);
		DemonAnim->AnimState = EMisterNegativeState::Idle;
		SetState(EMisterNegativeState::Idle);
		break;
	}
	// ���� ���¸� �� �� ������
	// ���� ���¿��� ���� ���·� �ѱ��.
}

void UMisterNegativeFSM::StaertState(EMisterNegativeState staertState)
{
	
	me->demonCol->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// ���� ���� ��ȯ
	switch (State)
	{
	case EMisterNegativeState::Idle:
		break;
	case EMisterNegativeState::Move:
		break;
	case EMisterNegativeState::Damage:
		break;
	case EMisterNegativeState::Groggy:
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
		me->SwordCol->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	case EMisterNegativeState::LightningstepAttack:
		me->SwordCol->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	case EMisterNegativeState::LightningstepAttack_Idle:
		break;
	case EMisterNegativeState::LightningstepAttack_Attack:
		me->SwordCol->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	case EMisterNegativeState::SpinAttack_idle:
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
		me->demonCol->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	case EMisterNegativeState::DemonAttack2_idle:
		break;
	case EMisterNegativeState::DemonAttack2_Move:
		break;
	case EMisterNegativeState::DemonAttack2_Attack:
		me->demonCol->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	default:
		break;
	}
}

void UMisterNegativeFSM::beforebeforeState(EMisterNegativeState currentState)
{
	curState = currentState;
	// ���� ����� ������Ʈ
}

