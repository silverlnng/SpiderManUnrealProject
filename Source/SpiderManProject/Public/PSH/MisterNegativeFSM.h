// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Containers/EnumAsByte.h"
#include "MisterNegativeFSM.generated.h"


UENUM(BlueprintType)
enum class EMisterNegativeState : uint8
{
	Idle,
	Move,
	
	Damage,
	evasion, 
	Die,

	Groggy, 
	Groggy_loop, 
	Groggy_end,

	Attack,

	LightningAttack,

	stepAttack_Idle,
	stepAttack_Attack,

	SpinAttack_idle,
	SpinAttack_Attack,
	
	ChargingAttack_idle,
	ChargingAttack_Attack,
	
	DemonAttack1_idle,
	DemonAttack1_Move,
	DemonAttack1_Attack,
	
	DemonAttack2_idle,
	DemonAttack2_Move,
	DemonAttack2_Attack,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPIDERMANPROJECT_API UMisterNegativeFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMisterNegativeFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	float curTime = 0;
public: // 기본 퍼블릭 변수
// 애니메이션
	UPROPERTY()
	class UMisterNegativeAnim* MisterAnim;

	UPROPERTY()
	class UDemonAnim* DemonAnim;

	UPROPERTY()
	class AMisterNegative* me;

	UPROPERTY()
	class ASpiderMan* Target;

// 상태 변수
	UPROPERTY(EditAnywhere, Category = State)
	float maxHp = 100;
	UPROPERTY(EditAnywhere, Category = State)
	float GrggyHitTime = 1;

	UPROPERTY(EditAnywhere, Category = State)
	int stamina = 0;

	UPROPERTY(EditAnywhere, Category = State)
	bool bisNextStage = false; // 페이즈 구별 불값

	bool bisGroggy; // Grrogy 중인지
	bool bisAttack; // Attack 중인지
	bool bisMaxPower; // 회피 상태를 들어가는 조건

	// 데몬 매쉬 이동값
	FVector demonMeshLocation;

// 기본 보안 변수
private:
	float curHp = maxHp;

	// 소환

public:
	TArray<class ASpawnMonster*> Monsters;
	void SetMonster(ASpawnMonster* monster);
	void DeadSpawnMonster();
	void DeleteMonster(ASpawnMonster* monster);

private:
	// 이동
	void GoForMove(float Time , EMisterNegativeState BeforState);
	
	void SetLocation(FVector TargetLocation);


	FVector worldCenter; // �� �߾� ��ǥ

	FVector TargetLoc;
	FVector MeLoc;
	FVector Dir;
	FVector StartLoc;
	FVector CurLoc;
	FVector EndLoc;

	FRotator MeRotation;

	float dist;
	float Alpha = 0;
	
		
public: // 피격

	int hitcount = 0;
	void Dameged(float damge, int MontageNum, float LaunchPower, FVector Dirction);
	

// 테스트 변수

public:
	UPROPERTY(EditAnywhere, Category = Test)
	float AttackDelayTime = 5;

	UPROPERTY(EditAnywhere, Category = Test)
	bool bisTest;

	UPROPERTY(EditAnywhere, Category = Test)
	float GroggyTime = 5;


private: // 스테이트 관련 변수
	UPROPERTY()
	EMisterNegativeState State = EMisterNegativeState::Idle;

	UPROPERTY()
	EMisterNegativeState curState = EMisterNegativeState::Idle;

	int curPage = 0;

private: // 스테이트 관련 함수
		void idleState();
		void MoveState();

		void DamageState();
		void evasionState();//회피

		void GroggyState();// 
		void Groggy_loopState();// 
		void Groggy_EndState();

		void DieState();

		void AttackState();
	
		void LightningAttackState();

		void stepAttack_IdleState();
		void StepAttackState_AttackState();

		void SpinAttackState_IdleState();
		void SpinAttackState_AttackState();

		void ChargingAttack_IdleState();
		void ChargingAttack_AttackState();

		void DemonAttack1_idleState();
		void DemonAttack1_MoveState();
		void DemonAttack1_AttackState();

		void DemonAttack2_idleState();
		void DemonAttack2_MoveState();
		void DemonAttack2_AttackState();

		// 공격 체크
		void RandomAttackCheak_1Page();
		void RandomAttackCheak_1To2Page();
		void RandomAttackCheak_2Page();

public:
		// 스테이트 전환
		void SetState(EMisterNegativeState NewState);
		void EndState(EMisterNegativeState endState);
		void StartState(EMisterNegativeState startState);
};
