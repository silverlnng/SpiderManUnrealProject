// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpawnMonsterFSM.generated.h"
UENUM(BlueprintType)
enum class ESpawnMonsterState : uint8
{
	Spawn,
	Idle,
	Move,
	RollAttack_Start,
	RollAttack_End,
	RollAttack_Attack,
	LushAttack_Start,
	LushAttack_End,
	Attack1,
	Attack2,
	die,
};

UENUM(BlueprintType)
enum class EMonsterJobState : uint8
{
	idle,
	Sword,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPIDERMANPROJECT_API USpawnMonsterFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USpawnMonsterFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Owner 관련 
	UPROPERTY()
	class ASpawnMonster * me;
	UPROPERTY()
	class ASpiderMan * Target;
	UPROPERTY()
	class USpawnMonsterAnim * anim;

	// EnumState 관련 
	ESpawnMonsterState State = ESpawnMonsterState::Spawn;

	EMonsterJobState jobState = EMonsterJobState::idle;

public:
	void SetState(ESpawnMonsterState curState);

	void EndAnim(ESpawnMonsterState endState);

	void SpawnState(const float& DeltaTime);
	void MoveState(const float& DeltaTime);
	void IdleState(const float& DeltaTime);

	void Attack1State();
	void Attack2State();
	void LushAttack_StartState();
	void LushAttack_EndState();

	void RandomAttack();

	UPROPERTY(EditAnywhere)
	float AttackRange = 200.0f;
	UPROPERTY(EditAnywhere)
	float AttackRadius = 50.0f;

	void RollAttack_StartState();
	void RollAttack_AttackState();
	void RollAttack_EndState();

	void Die();

	float spin = 0;
	float curSpin = 0;
	FRotator curRot;

	FVector Dir;
	FVector TargetLoc;
	FVector StartLoc;
	FVector CurLoc;
	FVector EndLoc;
	float dist;
	float Alpha = 0;


	void AttackCchek();

private:


	float curTime = 0;
	float SpawnTime = 2;
	
};
