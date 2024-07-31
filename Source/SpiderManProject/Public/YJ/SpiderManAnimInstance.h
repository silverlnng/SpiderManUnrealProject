// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SpiderManAnimInstance.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EAnimState : uint8
{
	IDLEAnim UMETA(DisplayName = "대기애니") ,
	ATTACKAnim  UMETA(DisplayName = "공격애니") ,
	DAMAGEAnim UMETA(DisplayName = "데미지애니") ,
	DIEAnim UMETA(DisplayName = "죽음애니")
};

UCLASS()
class SPIDERMANPROJECT_API USpiderManAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EAnimState AnimState = EAnimState::IDLEAnim;

	UFUNCTION()
	void SetAnimState(EAnimState nextState);
	
};
