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

void UMisterNegativeAnim::HitAnim()
{
	Montage_Play(RightHit);
}

void UMisterNegativeAnim::DeadAnim()
{
	Montage_Play(Dead);
}


void UMisterNegativeAnim::RealDeadAnim()
{
	Montage_Play(RealDead);
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

void UMisterNegativeAnim::AnimNotify_MisterAnimStart()
{
	misterNegative->MisterFSM->StartState(AnimState);
}

void UMisterNegativeAnim::AnimNotify_MisterSwordPowerOut()
{
	misterNegative->SwordNiagaraVisible(false);
}

void UMisterNegativeAnim::AnimNotify_MisterSwordPower()
{
	misterNegative->SwordNiagaraVisible(true);
}

void UMisterNegativeAnim::AnimNotify_MisterNextAnim()
{
	misterNegative->bisDemonAttack = false;
}

void UMisterNegativeAnim::AnimNotify_NextShake()
{
	misterNegative->NextShake();
}

void UMisterNegativeAnim::AnimNotify_NextFadeIn()
{
	misterNegative->NextFadeIn();
}

void UMisterNegativeAnim::AnimNotify_NextLevel()
{
	misterNegative->NextLevel();
}
