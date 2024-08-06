// Fill out your copyright notice in the Description page of Project Settings.


#include "YJ/SpiderFSMComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "PSH/MisterNegative.h"
#include "PSH/MisterNegativeFSM.h"
#include "YJ/SpiderMan.h"
#include "YJ/Cable.h"
#include "CableComponent.h"

// Sets default values for this component's properties
USpiderFSMComponent::USpiderFSMComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USpiderFSMComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Me = Cast<ASpiderMan>(GetOwner());
}


// Called every frame
void USpiderFSMComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	FString mystate = UEnum::GetValueAsString(State);

	//위치에 문자 띄우기
	DrawDebugString(GetWorld(),GetOwner()->GetActorLocation(),mystate, nullptr,FColor::Yellow,0,true);
	// ...
	switch ( State )
	{
	case EState::IDLE:		TickIdle(DeltaTime);		break;
	case EState::SWING:		TickSwing(DeltaTime);		break;
	case EState::DoubleJump:TickDoubleJump(DeltaTime);	break;
	case EState::ATTACK:	TickAttack(DeltaTime);		break;
	case EState::DAMAGE:	TickDamage(DeltaTime);		break;
	case EState::DIE:		TickDie(DeltaTime);			break;
	}
}

void USpiderFSMComponent::TickIdle(const float& DeltaTime)
{
	Me->GetCharacterMovement()->GravityScale=1.75f;
}

void USpiderFSMComponent::TickDoubleJump(const float& DeltaTime)
{
	//Me 를 타겟점으로 lerp하게 이동 => 이렇게 하는동안 은 중력영향안받게
	FVector CurrentLocation = FMath::Lerp(Me->GetActorLocation(), Me->DoubleTargetVector, DeltaTime*5.f);
	Me->GetCharacterMovement()->GravityScale =0.1f;
	Me->SetActorLocation(CurrentLocation);
	float dist = FVector::Dist(Me->GetActorLocation(),Me->DoubleTargetVector);

	float length = (Me->GetActorLocation() - Me->DoubleTargetVector).Size();
	Me->CableActor->CableComp->CableLength = length - 300;

	//다 도착하면 idle으로 다시
	if(dist<=30.f)
	{
		Me->GetCharacterMovement()->GravityScale=1.75f;
		State=EState::IDLE;
	}
	
}

void USpiderFSMComponent::TickAttack(const float& DeltaTime)
{
	// ECC_GameTraceChannel4 : 네거티브 채널
	
	
}

void USpiderFSMComponent::TickSwing(const float& DeltaTime)
{
	
}

void USpiderFSMComponent::TickDamage(const float& DeltaTime)
{
}

void USpiderFSMComponent::TickDie(const float& DeltaTime)
{
}

void USpiderFSMComponent::SetState(EState NextState)
{
	//EState prevState = State; //기존의 값을 저장해두기
	FString mystate = UEnum::GetValueAsString(NextState);
	UE_LOG(LogTemp, Warning, TEXT("SetState : %s"), *mystate);
	State=NextState;
}


