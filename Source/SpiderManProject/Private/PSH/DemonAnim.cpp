// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/DemonAnim.h"
#include "PSH/MisterNegative.h"

void UDemonAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto ownerPawn = TryGetPawnOwner();

	misterNegative = Cast<AMisterNegative>(ownerPawn);
}

void UDemonAnim::AnimNotify_SetBossDissolve()
{
	misterNegative->SetDissolveAnim();
}

void UDemonAnim::AnimNotify_BossDissolve()
{
	misterNegative->DissolveAnim();
}

void UDemonAnim::AnimNotify_BossCameraShake()
{
	misterNegative->CameraShake();
	misterNegative->MonsterSpawn();
	UE_LOG(LogTemp, Warning, TEXT(" Boss : CameraShake"));

}