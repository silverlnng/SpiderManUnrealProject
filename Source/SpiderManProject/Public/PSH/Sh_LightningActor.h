// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sh_LightningActor.generated.h"

UCLASS()
class SPIDERMANPROJECT_API ASh_LightningActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASh_LightningActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent * col;
	UPROPERTY(EditAnywhere)
	float speed = 1500;
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent * mesh;

	UPROPERTY(EditDefaultsOnly)
	class UParticleSystem * hitEffect;

	UPROPERTY(EditDefaultsOnly)
	class UNiagaraComponent * Naiagara;

		UFUNCTION()
	void OnComponentBeginOverlap( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex ,bool bFromSweep, const FHitResult& SweepResult);
};
