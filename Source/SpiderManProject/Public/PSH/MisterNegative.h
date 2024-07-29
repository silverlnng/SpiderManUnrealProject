// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MisterNegative.generated.h"

UCLASS()
class SPIDERMANPROJECT_API AMisterNegative : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMisterNegative();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	
	void SpawnLightning(); // LightningAttack Spawn
	void SpawnSpin(); // SpinAttack Spawn
	void SpawnCharging(); // ChargingAttack Spawn

	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent * Sword;
	UPROPERTY(EditAnywhere)
	class UMisterNegativeFSM * MisterFSM;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AActor>  Lightning;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AActor> Spin;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AActor> Charging;
private:
};
