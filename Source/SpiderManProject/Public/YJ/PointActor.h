// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PointActor.generated.h"

UCLASS()
class SPIDERMANPROJECT_API APointActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APointActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	class UBoxComponent* boxComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	class UStaticMeshComponent* meshComp;

};
