// Fill out your copyright notice in the Description page of Project Settings.


#include "PSH/SpawnMonsterAnim.h"
#include "PSH/SpawnMonster.h"

void USpawnMonsterAnim::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Me = Cast<ASpawnMonster>(TryGetPawnOwner());
}

void USpawnMonsterAnim::Die_Montage()
{
	Montage_Play(dieMontage);
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


void USpawnMonsterAnim::AnimNotify_SpawnLightning()
{
	Me->SpawnLightning();
}

void USpawnMonsterAnim::AnimNotify_MonsterAttack()
{
	Me->SwordAttack();
}

void USpawnMonsterAnim::AnimNotify_MonsterLeftAttack()
{
	Me->LeftAttack();
}

void USpawnMonsterAnim::AnimNotify_MonsterRightAttack()
{
	Me->RightAttack();
}	

void USpawnMonsterAnim::AnimNotify_StartDissolve()
{
	// ��� ���̰� �ϱ�
}

void USpawnMonsterAnim::AnimNotify_EndDissolve()
{
	Me->SetDissolveAnim();
	// dissolve �۵�
}

void USpawnMonsterAnim::AnimNotify_DieMonster()
{
	Me->Destroy();
	// ��������
}

void USpawnMonsterAnim::AnimNotify_EndAttack()
{
	Me->EndAttack();
}
