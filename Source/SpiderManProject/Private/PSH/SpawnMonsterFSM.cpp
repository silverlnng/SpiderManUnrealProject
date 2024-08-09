// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/SpawnMonsterFSM.h"
#include "PSH/SpawnMonster.h"
#include "YJ/SpiderMan.h"
#include "Kismet/GameplayStatics.h"
#include "PSH/SpawnMonsterAnim.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
USpawnMonsterFSM::USpawnMonsterFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// ...
}


// Called when the game starts
void USpawnMonsterFSM::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
	Target = Cast<ASpiderMan>(GetWorld()->GetFirstPlayerController()->GetCharacter());

	me = Cast<ASpawnMonster>(GetOwner());
	if (me != nullptr)
	{
		anim = Cast<USpawnMonsterAnim>(me->GetMesh()->GetAnimInstance());
	}

	int32 num = FMath::RandRange(1,2);
	
	switch (num)
	{
	case 1 :
		me->Setvisble(false);
		jobState = EMonsterJobState::idle;
		anim->jobAnimState = jobState;
		break;
	case 2 :
		me->Setvisble(true);
		jobState = EMonsterJobState::Sword;
		anim->jobAnimState = jobState;
		break;
	}
	
}


// Called every frame
void USpawnMonsterFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FString myState = UEnum::GetValueAsString(State);
	FString EnemyJobState = UEnum::GetValueAsString(jobState);

	DrawDebugString(GetWorld(), GetOwner()->GetActorLocation(), myState, nullptr, FColor::Yellow, 0, true, 1);
	DrawDebugString(GetWorld(), GetOwner()->GetActorLocation() + FVector(0,0,50), EnemyJobState, nullptr, FColor::Red, 0, true, 1);

	switch (State)
	{
	case ESpawnMonsterState::Spawn:
		SpawnState( DeltaTime);
		break;
	case ESpawnMonsterState::Idle:
		IdleState(DeltaTime);
		break;
	case ESpawnMonsterState::Move:
		MoveState(DeltaTime);
		break;
	case ESpawnMonsterState::Attack1:
		Attack1State();
		break;
	case ESpawnMonsterState::Attack2:
		Attack2State();
		break;
	case ESpawnMonsterState::RollAttack_Start:
		RollAttack_Start();
		break;
	case ESpawnMonsterState::RollAttack_End:
		RollAttack_End();
		break;
	case ESpawnMonsterState::RollAttack_Attack:
		RollAttack_Attack();
		break;
	case ESpawnMonsterState::LushAttack_Start:
		LushAttackState();
		break;
	}
	// ...
}

void USpawnMonsterFSM::SetState(ESpawnMonsterState curState)
{
	State = curState;
	anim->AnimState = State;
}

void USpawnMonsterFSM::EndAnim(ESpawnMonsterState endState)
{
	switch (State)
	{
	case ESpawnMonsterState::Spawn:
		break;
	case ESpawnMonsterState::Idle:
		break;
	case ESpawnMonsterState::Move:
		break;

	case ESpawnMonsterState::Attack1:
		SetState(ESpawnMonsterState::Idle);
		break;
	case ESpawnMonsterState::Attack2:
		SetState(ESpawnMonsterState::Idle);
		break;
	case ESpawnMonsterState::RollAttack_Start:
		UE_LOG(LogTemp, Warning, TEXT("RollEnd"));
		SetState(ESpawnMonsterState::RollAttack_Attack);
		break;
	case ESpawnMonsterState::RollAttack_End:
		SetState(ESpawnMonsterState::Attack1);
		break;
	case ESpawnMonsterState::RollAttack_Attack:
		break;
	case ESpawnMonsterState::LushAttack_Start:
		SetState(ESpawnMonsterState::Attack2);
		break;
	}
}

void USpawnMonsterFSM::SpawnState(const float& DeltaTime) 
{
	// 애니메이션 추가후 애니메이션이 끝나면 Idle로 이동.
	// 디졸브 실행.
	curTime += DeltaTime;

	if (curTime >= SpawnTime)
	{
		SetState(ESpawnMonsterState::Idle);
		curTime = 0;
	}
}

void USpawnMonsterFSM::IdleState(const float& DeltaTime)
{
	/*Target = Cast<ASpiderMan>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpiderMan::StaticClass()));*/
	

// 	if (Target)
// 	{
// 		SetState(ESpawnMonsterState::Move);
// 	}

	curTime += DeltaTime;

	if (curTime >= 1)
	{
		SetState(ESpawnMonsterState::Move);
		curTime = 0;
	}
}
void USpawnMonsterFSM::MoveState(const float& DeltaTime)
{
	FVector Destination = Target->GetActorLocation();
	FVector dir = Destination - me->GetActorLocation();
	float dist = dir.Size();

	me->AddMovementInput(dir.GetSafeNormal());

	if (dist < AttackRange) // 어택 가능 범위라면
	{
		RandomAttack(); // 랜덤 어택
		curTime = 0;
	}
}

void USpawnMonsterFSM::RandomAttack()
{

	int RandNum = FMath::RandRange(1,2);

	if (jobState == EMonsterJobState::idle)
	{
		switch (RandNum)
		{
		case 1:
			SetState(ESpawnMonsterState::Attack1);
			break;
		case 2:
			SetState(ESpawnMonsterState::Attack2);
			break;
		}
	}
	else
	{
		switch (RandNum)
		{
		case 1:
			SetState(ESpawnMonsterState::RollAttack_Start);
			break;
		case 2:
			SetState(ESpawnMonsterState::LushAttack_Start);
			break;
		}
	}
	

}

void USpawnMonsterFSM::RollAttack_Start()
{

}

void USpawnMonsterFSM::RollAttack_Attack()
{
	me->GetCharacterMovement()->bOrientRotationToMovement =false;
	spin -= GetWorld()->DeltaTimeSeconds * 70;
	curSpin += spin;
	me->AddActorLocalRotation(FRotator(spin,0,0));
	
	if (curSpin <= -360)
	{
			SetState(ESpawnMonsterState::RollAttack_End);
			curSpin = 0;
			spin =0;
	}
	
		// 회전
}

void USpawnMonsterFSM::RollAttack_End()
{
	me->GetCharacterMovement()->bOrientRotationToMovement = true;
	// 애니메이션 진행후 멈추고
}


void USpawnMonsterFSM::AttackCchek()
{
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, me);

	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		me->GetActorLocation(),
		me->GetActorLocation() + me->GetActorForwardVector() * AttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_Visibility,
		FCollisionShape::MakeSphere(AttackRadius),
		Params);

	FVector TraceVec = me->GetActorForwardVector() * AttackRange;
	FVector Center = me->GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = AttackRange * 0.5f + AttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 5.0f;

	 	DrawDebugCapsule(GetWorld(),
	 		Center,
	 		HalfHeight,
			AttackRadius,
	 		CapsuleRot,
	 		DrawColor,
	 		false,
			DebugLifeTime);

	if (bResult)
	{
		if (HitResult.GetActor()->IsValidLowLevel())
		{
			auto AttackTarget = Cast<ASpiderMan>(HitResult.GetActor());
			if (AttackTarget != nullptr)
			{
				AttackTarget->Damaged(1);
			}

		}
	}
}

void USpawnMonsterFSM::Attack1State()
{
	curTime += GetWorld()->DeltaTimeSeconds;

	if (curTime >= SpawnTime)
	{
		SetState(ESpawnMonsterState::Idle);
		curTime = 0;
	}
}

void USpawnMonsterFSM::Attack2State()
{
	curTime += GetWorld()->DeltaTimeSeconds;

	if (curTime >= SpawnTime)
	{
		SetState(ESpawnMonsterState::Idle);
		curTime = 0;
	}
}

void USpawnMonsterFSM::LushAttackState()
{

}



