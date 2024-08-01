// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MisterNegativeFSM.h"
#include "DemonAnim.generated.h"

/**
 * 
 */
UCLASS()
class SPIDERMANPROJECT_API UDemonAnim : public UAnimInstance
{
	GENERATED_BODY()
public:
		virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AttackFSM)
	class UMisterNegativeFSM * MisterFSM;

	class AMisterNegative * misterNegative;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AttackFSM)
	EMisterNegativeState AnimState;

	UFUNCTION()
	void AnimNotify_MisterAnimEnd();

	UFUNCTION()
	void AnimNotify_BossCameraShake();
	
};
