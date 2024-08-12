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

	UPROPERTY(EditDefaultsOnly)
	class UNiagaraComponent * Naiagara;

	UPROPERTY()
	class AMisterNegative * MisterNegative;

	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent * Sword;

	UPROPERTY(EditAnywhere)
	class UCapsuleComponent * SwordCol;

	UPROPERTY(EditAnywhere)
	class UCapsuleComponent * LeftCol;

	UPROPERTY(EditAnywhere)
	class UCapsuleComponent * RightCol;

	UPROPERTY(EditDefaultsOnly)
	class USpawnMonsterFSM * FSM;

	void Setvisble(bool chek);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AActor>  Lightning;


	void SpawnLightning(); // LightningAttack Spawn

	void SwordAttack();

	void RightAttack();
	void LeftAttack();

	void EndAttack();

	void SetDissolveAnim();
	void SetDissolveInit();

	void StopEffect();

	float dissolveAnimValue = 1;

	bool bisDissolve = false;

	bool SetisDissolve = false;

	UFUNCTION()
	void SwordComponentBeginOverlap( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex ,bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void LeftComponentBeginOverlap( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex ,bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void RightComponentBeginOverlap( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex ,bool bFromSweep, const FHitResult& SweepResult);

};
