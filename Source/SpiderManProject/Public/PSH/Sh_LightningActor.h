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
	float speed = 1000;
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent * mesh;
};
