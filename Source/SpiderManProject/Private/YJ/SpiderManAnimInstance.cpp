// Fill out your copyright notice in the Description page of Project Settings.


#include "YJ/SpiderManAnimInstance.h"

#include "YJ/SpiderFSMComponent.h"

void USpiderManAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void USpiderManAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	FString mystate = UEnum::GetValueAsString(AnimState);

	//위치에 문자 띄우기
	if (SpiderMan)
	{
	DrawDebugString(GetWorld(),SpiderMan->GetActorLocation(),mystate, nullptr,FColor::Black,0,true);
	}
	
}

void USpiderManAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	SpiderMan = Cast<ASpiderMan>(TryGetPawnOwner());
}

void USpiderManAnimInstance::SetAnimState(EAnimState nextState)
{
	AnimState = nextState;
}

void USpiderManAnimInstance::AnimNotify_SpiderAnimEnd()
{
	//AnimState = EAnimState::IDLEAnim;
	SetAnimState(EAnimState::IDLEAnim);
	if (SpiderMan->FSMComp)
	{
		SpiderMan->FSMComp->SetState(EState::IDLE);
	}
	
}
