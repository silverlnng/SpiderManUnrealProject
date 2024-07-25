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
	Attack,
	evasion, 
	Die,
	LightningAttack,
	StepAttack,
	SpinAttack,
	ChargingAttack
};

UENUM(BlueprintType)
enum class EMisterNegativeChargingAttackState : uint8
{
	Idle,
	Attack,
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

	UPROPERTY()
	EMisterNegativeChargingAttackState chargingAttackState = EMisterNegativeChargingAttackState::Idle;

	UPROPERTY()
	class UMisterNegativeAnim* MisterAnim;

	UPROPERTY()
	class AMisterNegative* me;
private:

	float curTime =0;

	float AttackDelayTime =2;

	void idleState();
	void MoveState();
	void DamageState();
	void GroggyState();// 
	void AttackState();
	void evasionState();// È¸ÇÇ
	void DieState();

	void RandomAttackCheak();

	void LightningAttackState();
	void StepAttackState();
	void SpinAttackState();
	void ChargingAttackState();
};
