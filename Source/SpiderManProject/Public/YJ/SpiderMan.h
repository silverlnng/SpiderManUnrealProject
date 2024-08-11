// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "SpiderMan.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class SPIDERMANPROJECT_API ASpiderMan : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASpiderMan();
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void PostInitializeComponents() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Setting")
	FVector2D MovementVector;
	
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Setting")
	class UCableComponent* CableComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Setting")
	bool canHook;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Setting")
	bool hooked;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Setting")
	FVector hookPoint;
	
	UFUNCTION(BlueprintCallable)
	void FindHookPoint_pushShift();

	UFUNCTION(BlueprintCallable)
	void CompletedHook();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting")
	UInputAction* LMouseAction; // 어택 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting")
	UInputAction* HookAction; //L - Shift
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	float MaxSwingTraceDistance = 10000.0f;

	UFUNCTION(BlueprintCallable)
	void CalculateSwing(FVector loc);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	float Radius = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	TSubclassOf<class ACable> BP_CableActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	class ACable* CableActor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	FVector force;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	class APlayerCameraManager* CameraManager;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	FTimerHandle addforceTimer;
	
	UFUNCTION(BlueprintCallable)
	void DetectWall(FVector Direction);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	float DetectTraceLength = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	bool DetctedWall;

	UFUNCTION(BlueprintCallable)
	void ClimbingMode();

	UFUNCTION(BlueprintCallable)
	void FindHookPoint_Auto();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	TArray<AActor*> HookPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	class AActor* Hookpoint_Nearest;

	float prevDistance = 500.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	float DetectHookPointRadius = 5000.0f;
	
	UPROPERTY()
	class APlayerController* pc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	class APhyConstraintActor* PConstraintActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	TSubclassOf<class APhyConstraintActor> BP_PhysicsConstraint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	class UPhysicsHandleComponent* PhysicsHandle;

	UFUNCTION(BlueprintCallable)
	TArray<AActor*> DetectEnemy();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	float DetectEnemyRadius = 1000.0f; // ���� �ݰ� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	float DetectionAngle = 45.0f; // ���� ���� ���� (90���� ��)

	UFUNCTION(BlueprintCallable)
	void MyDrawDebugLine();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	TSubclassOf<class APointActor> BP_StartPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	class APointActor* StartPointActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	TSubclassOf<class APointActor> BP_EndPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	class APointActor* EndPointActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	FVector camForce;

	virtual void Jump() override;
	virtual void StopJumping() override;

	UFUNCTION(BlueprintCallable)
	void DoubleJump();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USpiderFSMComponent* FSMComp;

	FVector DoubleTargetVector;

	UFUNCTION(BlueprintCallable)
	void Attack();

	UPROPERTY(EditDefaultsOnly)
	class USpiderManAnimInstance* SpiderManAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	float MaxHP=10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	float CurHP=MaxHP;

	UFUNCTION(BlueprintCallable)
	void Damaged(float value);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	FVector newforce;

	// 잡을 물건 detect 하기 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	float DetectRadius = 70.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "detect")
	float ConeAngle = 90.f; // 부채꼴의 전체 각도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "detect")
	float RayDetectObjDistance = 5000.f; // 레이의 길이
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "detect")
	int RayCount = 30;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CatchaObj")
	class AActor* CatchableObj;

	UFUNCTION(BlueprintCallable)
	void DetectCatchActor();
	UFUNCTION(BlueprintCallable)
	void CatchActor();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CatchaObj")
	float curtime=0;
	bool bRotateSpiderMan=false;
	UFUNCTION(BlueprintCallable)
	void RotateSpiderMan(float time);
	
	UFUNCTION(BlueprintCallable)
	void ThrowCatchActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	FTimerHandle addforceCatchItemTimer;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting")
	UInputAction* IA_CatchAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting")
	bool bPressedCatchObj=false;

//////////////////// 콤보 공격////////////////////////////////////////////////////////////////////

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "ComboAttack")
	bool IsAttacking;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "ComboAttack")
	bool CanNextCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "ComboAttack")
	bool IsComboInputOn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "ComboAttack")
	int32 CurrentCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "ComboAttack")
	int32 MaxCombo;

	UFUNCTION()
	void ComboAttack();

	UFUNCTION()
	void ComboAttackCheck();
	
	UFUNCTION()
	void AirAttackTriggerCheck();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Attack)
	float AttackRange =50.f ;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Attack)
	float AttackRadius = 50.f;
	
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void AttackStartComboState();
	void AttackEndComboState();
	
///////////////////////// 에어 콤보 ////////////////////////////////////
///
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ComboAirAttack")
	bool bCanAirAttackStart;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ComboAirAttack")
	bool IsAirAttacking;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ComboAirAttack")
	bool bCanAirNextCombo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ComboAirAttack")
	bool IsAirComboInputOn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ComboAirAttack")
	int32 CurrentAirCombo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ComboAirAttack")
	int32 MaxAirCombo;
	
	UFUNCTION()
	void AirComboAttack();

	UFUNCTION()
	void AirComboAttackEnded();
	


	
//////////////// 회피 기능	//////////// 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dodge")
	UInputAction* IA_LCtrl;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dodge")
	bool bIsDodging;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dodge")
	float DodgeDistance;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dodge")
	float DodgeCooldown;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dodge")
	float LastDodgeTime;
	
	UFUNCTION(BlueprintCallable)
	void Dodge(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void StopDodge();

/////////////////////////////////////보스 /////////////////////////////

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	class AMisterNegative* BossEnemy;
	
};
