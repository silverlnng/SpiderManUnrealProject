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
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Setting")
	class UCableComponent* ropeComp;
	
	UFUNCTION(BlueprintCallable)
	void FindHookPint();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setting")
	UInputAction* LMouseAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	float MaxTraceDistance = 2000.0f;

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
	float DetectTraceLength =50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	bool DetctedWall;
};
