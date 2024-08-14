// Fill out your copyright notice in the Description page of Project Settings.


#include "YJ/SpiderManAnimInstance.h"

#include "PSH/MisterNegative.h"
#include "PSH/MisterNegativeFSM.h"
#include "YJ/SpiderFSMComponent.h"

void USpiderManAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void USpiderManAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	/*FString mystate = UEnum::GetValueAsString(AnimState);

	//위치에 문자 띄우기
	if (Me)
	{
	DrawDebugString(GetWorld(),Me->GetActorLocation(),mystate, nullptr,FColor::Black,0,true);
	}*/

	if ( nullptr == Me )
	{
		return;
	}

	FVector velocity = Me->GetVelocity();
	FVector forwardVector = Me->GetActorForwardVector();
	FVector rightVector = Me->GetActorRightVector();

	// 앞 뒤 속도
	Speed = FVector::DotProduct(forwardVector , velocity);

	// 좌우 속도
	Direction = FVector::DotProduct(rightVector , velocity);

	bIsDodging = Me->bIsDodging;

	bIshooking = Me->hooked;
}

void USpiderManAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	Me = Cast<ASpiderMan>(TryGetPawnOwner());
}

void USpiderManAnimInstance::SetAnimState(EAnimState nextState)
{
	AnimState = nextState;
}

void USpiderManAnimInstance::AnimNotify_SpiderAnimEnd()
{
	//AnimState = EAnimState::IDLEAnim;
	SetAnimState(EAnimState::IDLEAnim);
	if (Me->FSMComp)
	{
		Me->FSMComp->SetState(EState::IDLE);
	}
	
}

void USpiderManAnimInstance::AnimNotify_SpiderAttack_Start()
{
	USkeletalMeshComponent* SkeletalMeshComp =Me-> FindComponentByClass<USkeletalMeshComponent>();

	// 손 본 이름을 설정합니다.
	FName HandBoneName_R = TEXT("hand_r");
	//FVector HandLocation_R = SkeletalMeshComp->GetBoneLocation(HandBoneName_R,EBoneSpaces::WorldSpace);
	FVector HandLocation_R = SkeletalMeshComp->GetSocketLocation(FName("hand_rSocket"));
	//
	
	FHitResult HitResult_Hand_R;
	
	FCollisionQueryParams Params;
	//Params.AddIgnoredActor(Me);
	
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult_Hand_R,
		Me->GetActorLocation(),
		Me->GetActorLocation() + Me->GetActorForwardVector() * AttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_Visibility,
		FCollisionShape::MakeSphere(AttackRadius),
		Params);

#if ENABLE_DRAW_DEBUG
	FVector TraceVec = Me-> GetActorForwardVector() * AttackRange;
	FVector Center = Me->GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = AttackRange * 0.5f + AttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 5.0f;

	DrawDebugCapsule(GetWorld(),
		Center,
		HalfHeight,
		AttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime);

#endif
	if (bResult) {
		if (::IsValid(HitResult_Hand_R.GetActor()))
		{
			UE_LOG(LogTemp,Warning, TEXT("Hit Actor Name : %s"), *HitResult_Hand_R.GetActor()->GetName());
			AMisterNegative* MisterNegative = Cast<AMisterNegative>(HitResult_Hand_R.GetActor());
			if(MisterNegative)
			{
				
				auto NegativeFSM = MisterNegative->GetComponentByClass<UMisterNegativeFSM>();
				NegativeFSM->Dameged(1, 1, 1000, MisterNegative->GetActorForwardVector() * -1);
			}
		}
	}
}


#pragma region ComboAttack

void USpiderManAnimInstance::PlayAttackMontage()
{
	if(!Montage_IsPlaying(ComboAttackMontage))
	{
		Montage_Play(ComboAttackMontage,1.f);
	}
}

void USpiderManAnimInstance::PlayAirAttackMontage()
{
	if(!Montage_IsPlaying(AirComboAttackMontage))
	{
		Montage_Play(AirComboAttackMontage,1.f);
	}
}


void USpiderManAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	if (Me->bCanAirAttackStart)
	{
		Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AirComboAttackMontage);
	}
	else
	{
		Montage_JumpToSection(GetAttackMontageSectionName(NewSection), ComboAttackMontage);
	}
	
	// 애니메이션 몽타주의  해당 세션으로 바로 플레이하는 내부함수 
}

void USpiderManAnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}

void USpiderManAnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

FName USpiderManAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	// 순서를 매개변수로 받아서 실행시킬 세션이름으로 반환
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}

void USpiderManAnimInstance::AnimNotify_AirAttackTriggerCheck()
{
	OnAirAttackTriggerCheck.Broadcast();
}

void USpiderManAnimInstance::AnimNotify_AirComboAttackEnded()
{
	OnAirAttackEnded.Broadcast();
}

void USpiderManAnimInstance::AnimNotify_DropKickEnded()
{
	DoubleJumpEnded =false;
}

void USpiderManAnimInstance::AnimNotify_GrabWebEnded()
{
	Me->FSMComp->SetState(EState::DoubleJump);
}

void USpiderManAnimInstance::PlayDamagedMontage()
{
	if(IsAnyMontagePlaying())
	{
		StopAllMontages(0.f);
	}
	Montage_Play(damageMontage,1.f);
}

#pragma endregion ComboAttack
