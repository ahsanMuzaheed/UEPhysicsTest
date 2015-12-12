// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CubeActor.generated.h"

UCLASS()
class PHYSICSTEST_API ACubeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACubeActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Harmonic Motion")
		float StartVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Harmonic Motion")
		float KElasticity;

private:
	float StartH;
	void DoPhysics(float DeltaTime);

	UStaticMeshComponent *mesh;
};
