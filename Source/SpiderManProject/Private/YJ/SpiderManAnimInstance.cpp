// Fill out your copyright notice in the Description page of Project Settings.


#include "YJ/SpiderManAnimInstance.h"

void USpiderManAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
}

void USpiderManAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
}

void USpiderManAnimInstance::SetAnimState(EAnimState nextState)
{
	AnimState = nextState;
}
