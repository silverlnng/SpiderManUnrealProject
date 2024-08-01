// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/DemonAnim.h"
#include "PSH/MisterNegative.h"

void UDemonAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto ownerPawn = TryGetPawnOwner();

	misterNegative = Cast<AMisterNegative>(ownerPawn);
}

void UDemonAnim::AnimNotify_MisterAnimEnd()
{

}
