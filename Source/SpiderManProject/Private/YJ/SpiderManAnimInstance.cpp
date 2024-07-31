// Fill out your copyright notice in the Description page of Project Settings.


#include "YJ/SpiderManAnimInstance.h"

void USpiderManAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	SpiderMan = Cast<ASpiderMan>(TryGetPawnOwner());
}

void USpiderManAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
}

void USpiderManAnimInstance::SetAnimState(EAnimState nextState)
{
	AnimState = nextState;
}

void USpiderManAnimInstance::AnimNotify_SpiderAnimEnd()
{
	//AnimState = EAnimState::IDLEAnim;
	SetAnimState(EAnimState::IDLEAnim);
}
