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
	SWING  UMETA(DisplayName = "스윙") ,
	DAMAGE UMETA(DisplayName = "데미지") ,
	DIE UMETA(DisplayName = "죽음")
};

UENUM(BlueprintType)
enum class ELevelState : uint8
{
	GMOVE UMETA(DisplayName = "일반이동시") ,
	GENEMY  UMETA(DisplayName = "일반에너미"),
	BOSSENEMY  UMETA(DisplayName = "보스에너미")
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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ELevelState LevelState = ELevelState::BOSSENEMY;
	
	UFUNCTION()
	void TickIdle(const float& DeltaTime);
	UFUNCTION()
	void TickDoubleJump(const float& DeltaTime);
	UFUNCTION()
	void TickAttack(const float& DeltaTime);
	UFUNCTION()
	void TickSwing(const float& DeltaTime);
	UFUNCTION()
	void TickDamage(const float& DeltaTime);
	UFUNCTION()
	void TickDie(const float& DeltaTime); 
	UFUNCTION()
	void SetState(EState NextState);
	
	UFUNCTION()
	void IdleState();
	
	UPROPERTY()
	class ASpiderMan* Me;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Attack)
	float AttackRange =200.f ;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Attack)
	float AttackRadius = 50.f;



	
};
