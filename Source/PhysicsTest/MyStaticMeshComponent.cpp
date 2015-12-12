// Fill out your copyright notice in the Description page of Project Settings.

#include "PhysicsTest.h"

#include "PhysXIncludes.h"
#include "PhysicsPublic.h"
#include "Runtime/Engine/Private/PhysicsEngine/PhysXSupport.h"

#include "MyStaticMeshComponent.h"

UMyStaticMeshComponent::UMyStaticMeshComponent()
{
	bWantsBeginPlay = true;

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	OnCalculateCustomPhysics.BindUObject(this, &UMyStaticMeshComponent::CustomPhysics);

	StartVelocity = 100.0f;
	KElasticity = 1.0f;

	// Floater
	lasterror = 0;
	errorsum = 0;

	SetTickGroup(ETickingGroup::TG_PrePhysics);
}

void UMyStaticMeshComponent::BeginPlay()
{
	Super::BeginPlay();

	StartH = GetComponentLocation().Z;
	SetPhysicsLinearVelocity(FVector(0.0f, 0.0f, StartVelocity));
}

void UMyStaticMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UE_LOG(LogClass, Log, TEXT("UMyStaticMeshComponent::TickComponent - DeltaTime: %f"), DeltaTime);

	if(!bSubstepEnabled)
		DoPhysics(DeltaTime, false);
	else 
	{
		// Add custom physics forces each tick
		GetBodyInstance()->AddCustomPhysics(OnCalculateCustomPhysics);
	}
}


void UMyStaticMeshComponent::CustomPhysics(float DeltaTime, FBodyInstance* BodyInstance)
{
	//UE_LOG(LogClass, Log, TEXT("CustomPhysics: %f"), DeltaTime);

	DoPhysics(DeltaTime, true);
}

void UMyStaticMeshComponent::DoPhysics(float DeltaTime, bool InSubstep)
{
	if (bUseFloater)
		DoFloater(DeltaTime, InSubstep);
	else
		DoHarmonic(DeltaTime, InSubstep);
}

void UMyStaticMeshComponent::DoHarmonic(float DeltaTime, bool InSubstep)
{
	float DeltaH = GetComponentLocation().Z - StartH;

	if (InSubstep) {
		physx::PxRigidBody* PRigidBody = GetBodyInstance()->GetPxRigidBody_AssumesLocked();
		PRigidBody->addForce(PxVec3(0.0f, 0.0f, -DeltaH * KElasticity), physx::PxForceMode::eFORCE, true);
	}
	else {
		AddForce(FVector(0.0f, 0.0f, -DeltaH * KElasticity));
	}

	UE_LOG(LogClass, Log, TEXT("UMyStaticMeshComponent::DoPhysics - deltaH %f, dt %f, mass %f, F %f"),
		GetComponentLocation().Z - StartH,
		DeltaTime,
		GetMass(),
		-DeltaH * KElasticity
		);
}

void UMyStaticMeshComponent::DoFloater(float DeltaTime, bool InSubstep)
{
	float prevError = lasterror;
	lasterror = GetComponentLocation().Z - StartH;
	errorsum += lasterror;

	float force = ((lasterror - prevError) * kd / DeltaTime + lasterror * kp + errorsum * ki) * power;

	if (InSubstep) {
		physx::PxRigidBody* PRigidBody = GetBodyInstance()->GetPxRigidBody_AssumesLocked();
		PRigidBody->addForce(PxVec3(0.0f, 0.0f, force), physx::PxForceMode::eFORCE, true);
	}
	else {
		AddForce(FVector(0.0f, 0.0f, force));
	}
	/*
	double prevError = RayHistory->LastError;
	RayHistory->LastError = TargetDistanceFromGround - RayHistory->TrackDistance;
	RayHistory->LastErrorF = RayHistory->LastError;
	RayHistory->ErrorSum = RayHistory->ErrorSum + RayHistory->LastError;

	floatSys.PrevForce = floatSys.Force;

	floatSys.Force = ((RayHistory->LastError - prevError) * floatSys.Kd / DeltaTick +
		RayHistory->LastError * floatSys.Kp +
		RayHistory->ErrorSum * floatSys.Ki)
		* floatSys.Power;
	*/
}