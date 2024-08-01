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

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Setting")
	bool canHook;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Setting")
	bool hooked;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Setting")
	FVector hookPoint;
	
	UFUNCTION(BlueprintCallable)
	void FindHookPint();

	UFUNCTION(BlueprintCallable)
	void CompletedHook();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting")
	UInputAction* LMouseAction; // 어택 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting")
	UInputAction* HookAction; //L - Shift
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	float MaxSwingTraceDistance = 10000.0f;

	UFUNCTION(BlueprintCallable)
	void CalculateSwing();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	float Radius = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	TSubclassOf<class ACable> BP_CableActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	class ACable* CableActor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	FVector force;

	UFUNCTION(BlueprintCallable)
	void DetectWall(FVector Direction);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	float DetectTraceLength =70;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	bool DetctedWall;

	UFUNCTION(BlueprintCallable)
	void ClimbingMode();

	UPROPERTY()
	class APlayerController* pc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	class APhyConstraintActor* PConstraintActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	TSubclassOf<class APhyConstraintActor> BP_PhysicsConstraint;

	

	UFUNCTION(BlueprintCallable)
	TArray<AActor*> DetectEnemy();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	float DetectionRadius = 1000.0f; // ���� �ݰ� ����
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

	virtual void Jump() override;
	virtual void StopJumping() override;

	UFUNCTION(BlueprintCallable)
	void DoubleJump();
	UPROPERTY()
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
	UPROPERTY()
	class AActor* CatchableObj;

	UFUNCTION(BlueprintCallable)
	void DetectCatchActor();
	UFUNCTION(BlueprintCallable)
	void CatchActor();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting")
	UInputAction* IA_CatchAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting")
	bool bPressedCatchObj=false;
	
};
