// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Cable.generated.h"

UCLASS()
class SPIDERMANPROJECT_API ACable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USceneComponent* SceneComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCableComponent* CableComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UPhysicsConstraintComponent* PhysicsConstraint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* StaticComp;

};
