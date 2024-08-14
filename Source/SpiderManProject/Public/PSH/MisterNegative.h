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
	UPROPERTY(EditAnywhere)
	class UMisterNegativeFSM* MisterFSM;
	

	void SpawnLightning(); // LightningAttack Spawn
	void SpawnSpin(); // SpinAttack Spawn
	void SpawnCharging(); // ChargingAttack Spawn

	UPROPERTY(EditAnywhere)
	class ANagetiveGameModeBase * Gmb;

	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent * Demon;

	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent * Sword;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AActor>  Lightning;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AActor> Spin;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AActor> Charging;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCameraShakeBase> Cs_DemonAttack;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCameraShakeBase> Cs_NextShake;

	UPROPERTY(EditAnywhere)
	class UWidgetComponent * SturnUi;
	UPROPERTY(EditAnywhere)
	class UCapsuleComponent * demonCol;
	UPROPERTY(EditAnywhere)
	class UCapsuleComponent * SwordCol;

	UPROPERTY(EditDefaultsOnly)
	class USoundBase * HitSound;

	UPROPERTY(EditDefaultsOnly)
	class AMonsterSpawner* SpawnMonster;

	UPROPERTY(EditDefaultsOnly)
	class UNiagaraComponent * Naiagara;

	void SwordNiagaraVisible(bool chek);

	void SetDemonMeshVisible(bool chek);

	void NextFadeIn();

	void CameraShake();

	void DissolveAnim();

	void SetDissolveInit();
	
	void NextLevel();

	void NextShake();

	void MonsterSpawn();

	void Ending();

	void SetUiVisble(bool chek); // SturnUi visible

	float dissolveAnimValue;

	bool bisDissolve;

	bool bisDemonAttack; // Up 애니메이션 재생
private:
	
	UFUNCTION()
	void SwordComponentBeginOverlap( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex ,bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void DemonComponentBeginOverlap( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex ,bool bFromSweep, const FHitResult& SweepResult);

};
