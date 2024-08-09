// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/SpawnMonsterAnim.h"
#include "PSH/SpawnMonster.h"

void USpawnMonsterAnim::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Me = Cast<ASpawnMonster>(TryGetPawnOwner());
}

void USpawnMonsterAnim::AnimNotify_NomalMonsterAttackEnd()
{
		UE_LOG(LogTemp, Warning, TEXT("End"));
	if (Me && Me->FSM)
	{
		UE_LOG(LogTemp, Warning, TEXT("End"));
		Me->FSM->EndAnim(AnimState);
	}
}
