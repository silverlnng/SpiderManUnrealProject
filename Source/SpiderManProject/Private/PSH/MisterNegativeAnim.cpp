// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/MisterNegativeAnim.h"
#include "PSH/MisterNegative.h"
#include "PSH/MisterNegativeFSM.h"

void UMisterNegativeAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto ownerPawn = TryGetPawnOwner();

	AmisterNegative = Cast<AMisterNegative>(ownerPawn);
}

void UMisterNegativeAnim::AnimNotify_MisterAnimEnd()
{
	AnimState = EMisterNegativeState::Idle;
	AmisterNegative->MisterFSM->State = AnimState;

	UE_LOG(LogTemp, Warning, TEXT("End - ganonDorfFSM->gState : %s"), *UEnum::GetValueAsString(AnimState));
	UE_LOG(LogTemp, Warning, TEXT("End - ganonDorfFSM->gState : %s"), *UEnum::GetValueAsString(AmisterNegative->MisterFSM->State));
	UE_LOG(LogTemp, Warning, TEXT("Notify"));
}

void UMisterNegativeAnim::AnimNotify_MisterNextAnim()
{
	bisNextAnim = true;
	UE_LOG(LogTemp, Warning, TEXT("MisterNextAnim"));
}
