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

void UMisterNegativeAnim::AnimNotify_SpawnLightning()
{
	misterNegative->SpawnLightning();
}

void UMisterNegativeAnim::AnimNotify_SpawnCharging()
{
	misterNegative->SpawnCharging();
}

void UMisterNegativeAnim::AnimNotify_SpawnSpin()
{
	misterNegative->SpawnSpin();
}

void UMisterNegativeAnim::AnimNotify_MisterAnimEnd()
{
	if(misterNegative != nullptr)
	misterNegative->MisterFSM->EndState(AnimState);
}

void UMisterNegativeAnim::AnimNotify_BossCameraShake()
{
	misterNegative->CameraShake();
	UE_LOG(LogTemp, Warning, TEXT("CameraShake"));

}

void UMisterNegativeAnim::AnimNotify_MisterNextAnim()
{
	misterNegative->bisDemonAttack = false;
}
