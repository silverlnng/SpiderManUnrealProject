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
	EMonsterJobState jobAnimState = EMonsterJobState::idle;

	UFUNCTION()
	void AnimNotify_NomalMonsterAttackEnd();
	

		
};
