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
	if(!Montage_IsPlaying(RightHit))
	{
		Montage_Play(RightHit,1.f);
	}
	
}

void UMisterNegativeAnim::JumpToAttackMontageSection(int32 NewSection)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,GetAttackMontageSectionName(NewSection).ToString());
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), RightHit);
}

FName UMisterNegativeAnim::GetAttackMontageSectionName(int32 Section)
{
	// 순서를 매개변수로 받아서 실행시킬 세션이름으로 반환
	return FName(*FString::Printf(TEXT("Damaged%d"), Section));
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
