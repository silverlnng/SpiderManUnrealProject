// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpiderMan.h"
#include "Animation/AnimInstance.h"
#include "SpiderManAnimInstance.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EAnimState : uint8
{
	IDLEAnim UMETA(DisplayName = "대기애니") ,
	ATTACKAnim  UMETA(DisplayName = "공격애니") ,
	SWINGATTACKAnim  UMETA(DisplayName = "스윙공격애니") ,
	DAMAGEAnim UMETA(DisplayName = "데미지애니") ,
	SwingAnim UMETA(DisplayName = "스윙애니") ,
	DIEAnim UMETA(DisplayName = "죽음애니")
};

DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAirAttackTriggerCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAirAttackEndedDelegate);
UCLASS()
class SPIDERMANPROJECT_API USpiderManAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeBeginPlay() override;

	// Vertical
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Speed;

	// Horizontal
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Direction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bIsDodging;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bIshooking;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EAnimState AnimState = EAnimState::IDLEAnim;

	UFUNCTION()
	void SetAnimState(EAnimState nextState);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class ASpiderMan* Me;

	UFUNCTION()
	void AnimNotify_SpiderAnimEnd();
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Attack)
	float AttackRange =50.f ;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Attack)
	float AttackRadius = 50.f;

/////////////////////////////////////////콤보 공격
	
	FOnNextAttackCheckDelegate OnNextAttackCheck;
	FOnAttackHitCheckDelegate OnAttackHitCheck;
	FOnAirAttackTriggerCheckDelegate OnAirAttackTriggerCheck;
	FOnAirAttackEndedDelegate OnAirAttackEnded;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ComboAttack");
	class UAnimMontage* ComboAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ComboAttack");
	class UAnimMontage* AirComboAttackMontage;

	UFUNCTION()
	void PlayAttackMontage();

	UFUNCTION()
	void PlayAirAttackMontage();
	
	UFUNCTION()
	void JumpToAttackMontageSection(int32 NewSection);
	
	UFUNCTION()
	void AnimNotify_AttackHitCheck();
	UFUNCTION()
	void AnimNotify_NextAttackCheck();
	UFUNCTION()
	FName GetAttackMontageSectionName(int32 Section);

	UFUNCTION()
	void AnimNotify_AirAttackTriggerCheck();
	void AnimNotify_AirComboAttackEnded();

///////////////////// DropKick //////////////////////////	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DropKick");
	class UAnimMontage* DoubleJumpAttackMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DropKick");
	bool DoubleJumpEnded =false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DropKick");
	bool DoubleStarted =false;

	UFUNCTION()
	void AnimNotify_DropKickEnded();

	UFUNCTION()
	void AnimNotify_GrabWebEnded();

/////////////// Dodge /////////////////////////////

	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* dodgeMontage;

/////////////// DAMAGE//////////////
	UFUNCTION()
	void PlayDamagedMontage();

	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* damageMontage;
	
	
};


