// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MisterNegativeFSM.h"
#include "MisterNegativeAnim.generated.h"

/**
 * 
 */
UCLASS()
class SPIDERMANPROJECT_API UMisterNegativeAnim : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AttackFSM)
	class UMisterNegativeFSM * MisterFSM;

	class AMisterNegative * misterNegative;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AttackFSM)
	EMisterNegativeState AnimState;

	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage * RightHit;
	
	void HitAnim();

	UFUNCTION()
	void AnimNotify_SpawnLightning();
	UFUNCTION()
	void AnimNotify_SpawnCharging();
	UFUNCTION()
	void AnimNotify_SpawnSpin();

	UFUNCTION()
	void AnimNotify_MisterAnimEnd();
	UFUNCTION()
	void AnimNotify_MisterAnimStart();

	UFUNCTION()
	void AnimNotify_MisterSwordPowerOut();
	UFUNCTION()
	void AnimNotify_MisterSwordPower();
	UFUNCTION()
	void AnimNotify_MisterNextAnim();
};
