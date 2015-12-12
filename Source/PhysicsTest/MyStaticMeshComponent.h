// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/StaticMeshComponent.h"
#include "MyStaticMeshComponent.generated.h"

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Harmonic Motion")
		float StartVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Harmonic Motion")
		float KElasticity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floater")
		bool bUseFloater;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floater")
		float kp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floater")
		float ki;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floater")
		float kd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floater")
		float power;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
		bool bSubstepEnabled;

private:
	float StartH;
	float lasterror, errorsum;

	void DoPhysics(float DeltaTime, bool InSubstep);
	void DoHarmonic(float DeltaTime, bool InSubstep);
	void DoFloater(float DeltaTime, bool InSubstep);
};
