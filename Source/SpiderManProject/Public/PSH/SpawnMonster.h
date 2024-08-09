// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpawnMonster.generated.h"

UCLASS()
class SPIDERMANPROJECT_API ASpawnMonster : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASpawnMonster();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent * Sword;

	UPROPERTY(EditAnywhere)
	class UCapsuleComponent * SwordCol;

	UPROPERTY(EditDefaultsOnly)
	class USpawnMonsterFSM * FSM;

	void Setvisble(bool chek);

};
