// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/MisterNegativeAnim.h"
#include "PSH/MisterNegative.h"
#include "PSH/MisterNegativeFSM.h"

void UMisterNegativeAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto ownerPawn = TryGetPawnOwner();

	misterNegative = Cast<AMisterNegative>(ownerPawn);
}

void UMisterNegativeAnim::AnimNotify_MisterAnimEnd()
{
	misterNegative->MisterFSM->EndState(AnimState);
}

void UMisterNegativeAnim::AnimNotify_MisterNextAnim()
{
	
}
