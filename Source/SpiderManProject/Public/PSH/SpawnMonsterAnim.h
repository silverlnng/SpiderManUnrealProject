// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SpawnMonsterFSM.h"
#include "SpawnMonsterAnim.generated.h"

/**
 * 
 */
UCLASS()
class SPIDERMANPROJECT_API USpawnMonsterAnim : public UAnimInstance
{
	GENERATED_BODY()

public:

	UPROPERTY()
	class ASpawnMonster * Me;

	virtual void NativeInitializeAnimation() override;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	ESpawnMonsterState AnimState = ESpawnMonsterState::Spawn;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	class UAnimMontage * dieMontage;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	EMonsterJobState jobAnimState = EMonsterJobState::idle;

	void Die_Montage();

	UFUNCTION()
	void AnimNotify_NomalMonsterAttackEnd();

	UFUNCTION()
	void AnimNotify_MonsterAttack();

	UFUNCTION()
	void AnimNotify_SpawnLightning();

	UFUNCTION()
	void AnimNotify_MonsterLeftAttack();

	UFUNCTION()
	void AnimNotify_MonsterRightAttack();
	
	UFUNCTION()
	void AnimNotify_StartDissolve();
	UFUNCTION()
	void AnimNotify_EndDissolve();
	UFUNCTION()
	void AnimNotify_DieMonster();
	UFUNCTION()
	void AnimNotify_EndAttack();

		
};
