// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/StaticMeshComponent.h"
#include "MyStaticMeshComponent.generated.h"

class ACubeActor;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PHYSICSTEST_API UMyStaticMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
	
public:
	UMyStaticMeshComponent();

	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FCalculateCustomPhysics OnCalculateCustomPhysics;

	void CustomPhysics(float DeltaTime, FBodyInstance* BodyInstance);

private:
	ACubeActor *owner;
};
