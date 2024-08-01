// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpiderFSMComponent.generated.h"

UENUM(BlueprintType)
enum class EState : uint8
{
	IDLE UMETA(DisplayName = "대기") ,
	DoubleJump  UMETA(DisplayName = "이동") ,
	ATTACK  UMETA(DisplayName = "공격") ,
	DAMAGE UMETA(DisplayName = "데미지") ,
	DIE UMETA(DisplayName = "죽음")
};
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPIDERMANPROJECT_API USpiderFSMComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USpiderFSMComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EState State = EState::IDLE;

	void TickIdle(const float& DeltaTime);
	void TickDoubleJump(const float& DeltaTime);
	void TickAttack(const float& DeltaTime);
	void TickDamage(const float& DeltaTime);
	void TickDie(const float& DeltaTime); 

	void SetState(EState NextState);
	
	UPROPERTY()
	class ASpiderMan* Me;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Attack)
	float AttackRange =200.f ;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Attack)
	float AttackRadius = 50.f;
};
