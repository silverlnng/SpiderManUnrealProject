// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MisterNegativeFSM.generated.h"


UENUM(BlueprintType)
enum class EMisterNegativeState : uint8
{
	Idle,
	Move,
	Damage,
	Groggy, 
	Groggy_loop, 
	Attack,
	evasion, 
	Die,
	LightningAttack,
	LightningstepAttack,
	LightningstepAttack_Idle,
	LightningstepAttack_Attack,
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

	UPROPERTY()
	EMisterNegativeState State = EMisterNegativeState::Idle;

// 	UPROPERTY()
// 	class ASpiderMan* Target;
	UPROPERTY()
	class ASpiderMan* Target;

	UPROPERTY()
	class UMisterNegativeAnim* MisterAnim;
	UPROPERTY()
	class UDemonAnim* DemonAnim;

	UPROPERTY()
	class AMisterNegative* me;
private:

	float curTime =0;

	float AttackDelayTime =1;

	int curPage = 0;

	FVector Dir;
	FVector TargetLoc;
	FVector StartLoc;
	FVector CurLoc;
	FVector EndLoc;
	FVector MeLoc;
	FRotator MeRotation;
	
	float curHp = maxHp;
	
	float dist;
	float Alpha = 0;

	void idleState();
	void MoveState();
	void DamageState();
	void GroggyState();// 
	void Groggy_loopState();// 
	void AttackState();
	void evasionState();// È¸ÇÇ
	void DieState();

	void RandomAttackCheak1();
	void RandomAttackCheak2();

	void LightningAttackState();

	void LightningStepAttackState();
	void LightningstepAttack_IdleState();
	void LightningStepAttackState_stepAttackState();

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
		
	

	void SetState(EMisterNegativeState NewState);

	UPROPERTY(EditAnywhere, Category = State)
	float maxHp = 100;

	UPROPERTY(EditAnywhere, Category = State)
	int stamina = 0;

	UPROPERTY(EditAnywhere, Category = State)
	bool bisNextStage = false;

	FVector demonMeshLocation;
public:

	void Dameged(float damge);
	void EndState(EMisterNegativeState endState);
	void StaertState(EMisterNegativeState StaertState);
	void beforebeforeState(EMisterNegativeState curState);
};
